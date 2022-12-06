#include "defs.h"

int main(int argc, char *argv[]){

    int hunterSleepTime = 0;
    int ghostSleepTime = 0;
    if(argc == 3){
        hunterSleepTime = atoi(argv[1]);
        ghostSleepTime = atoi(argv[2]);
    }
    // Initialize a random seed for the random number generators
    srand(time(NULL));

    //Ghost thread
    pthread_t pThreadghost;

    // Initialize the building
    BuildingType building;
    initBuilding(&building);
    populateRooms(&building);

    // Create list of hunters
    HunterListType hunterList;
    HunterListType* hunterListPtr = &hunterList;
    initHunterList(hunterListPtr);

    // Ghost 
    initGhost(randInt(0,4), getRandomRoom(building.rooms->head)->data, ghostSleepTime, building.ghost);
    GhostType *ghostPtr = building.ghost;

    RoomType* vanRoom = building.rooms->head->data;
    
    // Hunters
    int toolSize = MAX_HUNTERS;
    int toolArray[MAX_HUNTERS] = {0,1,2,3};

    for(int i = 0; i < MAX_HUNTERS; i++) {
        char name[MAX_STR];
        printf("%d. Hunter: \n", i+1);
        // Input the name
        scanf("%s", name);
    
        // Make a new hunter
        HunterType *currentHunterPtr;
        // Initalize the hunter with their name, the van, a unique random tool, and the address
        initHunter(name, vanRoom, getUniqueRandomEvidenceTool(toolArray, &toolSize), hunterSleepTime, &currentHunterPtr);
        //Adding the hunters to the van, the main control flow list, and the building
        addHunterToRoom(vanRoom, currentHunterPtr);
        addHunterToList(hunterListPtr, currentHunterPtr);
        addHunterToList(building.hunters, currentHunterPtr);
    }

    // Populate the hunter thread array
    pthread_t hunterThreadArray[MAX_HUNTERS];
    for(int i = 0; i<MAX_HUNTERS; i++){
        pthread_t currentThread;
        hunterThreadArray[i] = currentThread;
    }
    
    // Create ghost thread
    pthread_create(&pThreadghost, NULL, ghostThread, (void*) ghostPtr);

    // Create hunter threads
    for(int i = 0; i<MAX_HUNTERS; i++) {
        pthread_create(hunterThreadArray + i, NULL, hunterThread, (void*) hunterListPtr->hunterList[i]);
    }

    // Join hunter threads
    for(int i = 0; i<MAX_HUNTERS; i++) {
        pthread_join(hunterThreadArray[i], NULL);
    }

    // Join ghost threads
    pthread_join(pThreadghost, NULL);
    
    // Print each hunter
    for(int i = 0; i<hunterListPtr->size; i++){
        printHunter(hunterListPtr->hunterList[i]);
    }

    // Gets the number of hunters with a fear greater than or equal to 100 & prints them
    printf("\n\nhunters with fear >= 100:\n");
    int fearCounter = getFear(hunterListPtr);
    // Determine the winner
    determineWinner(hunterListPtr, ghostPtr, fearCounter);

    // Free each hunter created with all their dynamically allocated memory
    for(int i = 0; i<hunterListPtr->size; i++){
        freeHunter(hunterListPtr->hunterList[i]);
    }

    // Free the rooms of the building
    freeRoomList(building.rooms);

    // Free the building data
    free(building.rooms);
    free(building.hunters);
    free(building.ghost);

    return 0;
}

/* ********************************************************************************************************************************
 * getFear, gets the number of hunters with max fear
 * int a (in), min value
 * int b (in), max value
 **********************************************************************************************************************************/
int getFear(HunterListType *hunterListPtr){
    int fearCounter = 0;
    // Iterate through the hunter list
    for(int i = 0; i < hunterListPtr->size; i++){
        // If the current hunter has their fear maxed print the hunter and increase the fearCounter
        if(hunterListPtr->hunterList[i]->fear >= MAX_FEAR){
            printHunter(hunterListPtr->hunterList[i]);
            fearCounter++;
        }
    }
    // Return the fear counter
    return fearCounter;
}

/* ********************************************************************************************************************************
 * randInt, returns a random integer between a and b-1
 * int a (in), min value
 * int b (in), max value
 **********************************************************************************************************************************/
int randInt(int min, int max) {
    return rand() % (max - min) + min;
}

/* ********************************************************************************************************************************
 * randFloat, returns a random float between a and b-1
 * float a (in), min value
 * float b (in), max value
 **********************************************************************************************************************************/
float randFloat(float a, float b) {
    // Get a percentage between rand() and the maximum
    float random = ((float) rand()) / (float) RAND_MAX;
    // Scale it to the range we want, and shift it
    return random * (b - a) + a;
}

/* ********************************************************************************************************************************
 * getUniqueRandomEvidenceTool, finds a unique piece of evidence from a list, removes it from the list then returns what was found
 * int *arr (in), the list of pieces of evidence
 * int *size (in), the size of arr
 **********************************************************************************************************************************/
int getUniqueRandomEvidenceTool(int *arr, int *size) {
    // Creating a variable to avoid repeatedly dereferencing
    int derefSize = *size;

    // Selecting a randomIndex
    int randomIndex = randInt(0, derefSize);
    // Saving the value at the index
    int valToReturn = arr[randomIndex];

    // Removing the value which was randomly selected from the array
    for(int i = 0; i<derefSize; i++){
        if(arr[i] == valToReturn){
            for(int j = i+1; j<derefSize; j++){
                arr[j-1] = arr[j];
            }
        }
    }

    // Reduce the size of the array
    (*size)--;

    //Return the value
    return valToReturn;
}

/* ********************************************************************************************************************************
 * determineGhost, calculates which ghost was found using the hunters' collected evidence and returns it
 * HunterListType *hunters (in), the list of hunters
 **********************************************************************************************************************************/
int determineGhost(HunterListType *hunters) {
    int emfCheck = C_FALSE;
    int temperatureCheck =  C_FALSE;
    int fingerprintsCheck = C_FALSE;
    int soundCheck = C_FALSE;

    for(int i = 0; i < hunters->size; i++){
        EvidenceNodeType *tempEvidenceNode = hunters->hunterList[i]->personalEvidence->head;
        while(tempEvidenceNode != NULL){
            if(isGhostly(tempEvidenceNode->data) && hunters->hunterList[i]->fear<100){
                switch(tempEvidenceNode->data->evidenceCategory){
                    case 0: // EMF
                        emfCheck = C_TRUE;
                        break;
                    case 1: // TEMPERATURE
                        temperatureCheck = C_TRUE;
                        break;
                    case 2: // FINGERPRINTS
                        fingerprintsCheck = C_TRUE;
                        break;
                    case 3: // SOUND
                        soundCheck = C_TRUE;
                        break;
                }
            }
            tempEvidenceNode = tempEvidenceNode->next;
        }
    }

    if(emfCheck && temperatureCheck && fingerprintsCheck){
        return POLTERGEIST;
    }else if(emfCheck && temperatureCheck && soundCheck){
        return BANSHEE;
    }else if(emfCheck && fingerprintsCheck && soundCheck){
        return BULLIES;
    }else if(temperatureCheck && fingerprintsCheck && soundCheck){
        return PHANTOM;
    }else{
        return UNKNOWN_GHOST;
    }
}

/* ********************************************************************************************************************************
 * determineWinner, prints which case has occured
 * HunterListType *list (in), the list of hunters to determine the ghost
 * GhostType *ghost (in), the ghost in the building
 * int fear (in), the number of players who have max fear
 **********************************************************************************************************************************/
void determineWinner(HunterListType *list, GhostType *ghost, int fear){
    // If the fear is greater than four, all hunters have max fear and the ghost won
    if(fear >= 4){
        printf("The ghost won\n");
        printGhost(ghost);
    // Other wise the hunters may have won
    } else {
        // Get the speculatedGhost from determineGhost
        GhostClassType speculatedGhost = (GhostClassType) determineGhost(list);
        // If the value of speculatedGhost is NOT UNKNOWN_GHOST then we've guessed the ghost type correctly
        if(speculatedGhost != UNKNOWN_GHOST){
            // Print the speculated type and the actual type
            printf("Speculated Ghost Type: %s\n", ghostTypeToString(speculatedGhost));
            printf("Actual Ghost Type: %s\n", ghostTypeToString(ghost->ghostType));  
        // If the ghost is UNKNOWN_GHOST let the user know that not enough evidence was collected
        }else{
            printf("There was not enough ghostly evidence collected to determine the ghost\n");
        }
    }
}
