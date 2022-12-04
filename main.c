
#include "defs.h"


int main(int argc, char *argv[])
{
    // Initialize a random seed for the random number generators
    for(int p = 0; p<1; p++){

        srand(time(NULL));

        //Ghost thread
        pthread_t pThreadghost;

        // You may change this code; this is for demonstration purposes
        BuildingType building;
        initBuilding(&building);
        populateRooms(&building);


        //Place hunters
        HunterListType hunterList;
        HunterListType* hunterListPtr = &hunterList;
        initHunterList(hunterListPtr);

        // Ghost 
        initGhost(randInt(0,4), getRandomRoom(building.rooms->head)->data, building.ghost);
        GhostType *ghostPtr = building.ghost;

        RoomType* vanRoom = building.rooms->head->data;
        
        int toolSize = MAX_HUNTERS;
        int toolArray[MAX_HUNTERS] = {0,1,2,3};

        

        for(int i = 0; i < MAX_HUNTERS; i++) {
            char name[MAX_STR];
            printf("%d. Hunter: \n", i+1);
            sprintf(name, "%d", i+1);
            // scanf("%s", name);
        
            HunterType *currentHunterPtr;
            initHunter(name, vanRoom, getUniqueRandomEvidenceTool(toolArray, &toolSize), &currentHunterPtr);
            addHunterToRoom(vanRoom, currentHunterPtr);
            addHunterToList(hunterListPtr, currentHunterPtr);
            addHunterToList(building.hunters, currentHunterPtr);
            
        }


        //Populate a hunter thread array
        pthread_t hunterThreadArray[MAX_HUNTERS];
        for(int i = 0; i<MAX_HUNTERS; i++){
            pthread_t currentThread;
            hunterThreadArray[i] = currentThread;

        }

        pthread_create(&pThreadghost, NULL, ghostThread, (void*) ghostPtr);

        for(int i = 0; i<MAX_HUNTERS; i++) {
            pthread_create(hunterThreadArray + i, NULL, hunterThread, (void*) hunterListPtr->hunterList[i]);
        }


        for(int i = 0; i<MAX_HUNTERS; i++) {
            pthread_join(hunterThreadArray[i], NULL);
        }

        pthread_join(pThreadghost, NULL);
        


        printf("\t----------------------------------------------------------------------------------------------------------------\n");
        printf("%55sFinal Results%35s\n", "", "");
        printf("\t----------------------------------------------------------------------------------------------------------------\n");
        for(int i = 0; i<hunterListPtr->size; i++){
            HunterType *currHunter = hunterListPtr->hunterList[i];
            printf("\tName: %-25s", currHunter->name);
        }
        printf("\n");
        for(int i = 0; i<hunterListPtr->size; i++){
            HunterType *currHunter = hunterListPtr->hunterList[i];
            printf("\tEvidence: %-20s", evidenceTypeToString(currHunter->evidence));
        }
        printf("\n");
        for(int i = 0; i<hunterListPtr->size; i++){
            HunterType *currHunter = hunterListPtr->hunterList[i];
            printf("\tRoom: %-25s", currHunter->room->name);
        }
        printf("\n");
        for(int i = 0; i<hunterListPtr->size; i++){
            HunterType *currHunter = hunterListPtr->hunterList[i];
            printf("\tfear: %-25d", currHunter->fear);
        }
        printf("\n");
        for(int i = 0; i<hunterListPtr->size; i++){
            HunterType *currHunter = hunterListPtr->hunterList[i];
            printf("\tBoredom: %-20d", currHunter->timer);
        }


        for(int i = 0; i<hunterListPtr->size; i++){
            printHunter(hunterListPtr->hunterList[i]);

        }
    
        for(int i = 0; i<hunterListPtr->size; i++){
            freeHunter(hunterListPtr->hunterList[i]);
        }

        freeRoomList(building.rooms);
        free(building.rooms);
        free(building.hunters);
        free(building.ghost);


            

        // int fearCounter = 0;
        // printf("hunters with fear >= 100:\n");
        // for(int i = 0; i < hunterListPtr->size; i++){
        //     if(hunterListPtr->hunterList[i]->fear >= 100){
        //         printHunter(hunterListPtr->hunterList[i]);
        //         fearCounter++;
        //     }
        // }

        // if(fearCounter >= 4){
        //     printf("The ghost won\n");
        //     printGhost(ghostPtr);
        // } else {
        //     // determineGhost(hunterListPr);
        // }

    }
    

    return 0;
}

/*
  Function:  randInt
  Purpose:   returns a pseudo randomly generated number, 
             in the range min to (max - 1), inclusively
       in:   upper end of the range of the generated number
   return:   randomly generated integer in the range [min, max-1) 
*/
int randInt(int min, int max)
{
    return rand() % (max - min) + min;
}

/*
  Function:  randFloat
  Purpose:   returns a pseudo randomly generated number, 
             in the range min to max, inclusively
       in:   upper end of the range of the generated number
   return:   randomly generated integer in the range [0, max-1) 
*/
float randFloat(float a, float b) {
    // Get a percentage between rand() and the maximum
    float random = ((float) rand()) / (float) RAND_MAX;
    // Scale it to the range we want, and shift it
    return random * (b - a) + a;
}

int getUniqueRandomEvidenceTool(int * arr, int *size){
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


// void determineGhost(HunterListType *hunters){
//     printf("DETERMINE THE GOD DAMN GHOST HERE\n");
//     char EMFcheck = 0 
//     char TEMPERATUREcheck =  0 
//     char FINERPRINTScheck = 0 
//     char SOUNDcheck = 0 

//     for(int i = 0; i < hunters->size; i++){
//         EvidenceNodeType *tempEvidenceNode = hunters->hunterList[i]->personalEvidence->head;
//         while(tempEvidenceNode != NULL){
//             if(isGhostly(tempEvidenceNode->data->readingData)){
//                 switch(tempEvidenceNode->data->evidenceCategory){
//                     case 0: // EMF
//                         EMFcheck = 1;
//                         break;
//                     case 1: // TEMPERATURE
//                         TEMPERATUREcheck = 1;
//                         break;
//                     case 2: // FINGERPRINTS
//                         FINGERPRINTScheck = 1;
//                         break;
//                     case 3: // SOUND
//                         SOUNDcheck = 1;
//                         break;
//                 }
//             }
//             tempEvidenceNode = tempEvidenceNode->next;
//         }
//     }
// 
//     
//     return;
// }

