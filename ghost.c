// Include defs.h
#include "defs.h"


/* *******************************************************************************************
 * initGhost, initalizes the given ghost
 * GhostClassType (in) the type of ghost
 * RoomType *room (in) the room the ghost exists in
 * GhostType *ghost (out), the ghost pointer to set for each initalization
 ********************************************************************************************/
void initGhost(GhostClassType ghostType, RoomType *room, int sleepTime, GhostType *ghost) {
    ghost->ghostType = ghostType;
    ghost->room = room;
    ghost->boredomTimer = BOREDOM_MAX;
    ghost->sleepTime = sleepTime;
}

/* *******************************************************************************************
 * initGhostList, initalizes the given ghost evidence list
 * GhostEvidenceListType *ghostEvidenceList (out), used to set the head & tail to NULL
 ********************************************************************************************/
void initGhostList(GhostEvidenceListType *ghostEvidenceList){
    // Initalize the head & tail to NULL
    ghostEvidenceList->head = NULL;
    ghostEvidenceList->tail = NULL;
}

/* *******************************************************************************************
 * getRandomRoom, gets a random room, given the head
 * RoomNodeType *head (in), given the head find a random room for the ghost
 ********************************************************************************************/
RoomNodeType* getRandomRoom(RoomNodeType *head){
    // Generate a random int from 1-TOTAL_ROOMS (do not include 0, because that is the van)
    int randomRoomInt = randInt(1, TOTAL_ROOMS);
    RoomNodeType *traverseRoomNode = head;
    for(int i = 0; i<randomRoomInt; i++){
        traverseRoomNode = traverseRoomNode->next;
    }
    //Return the random node
    return traverseRoomNode;
}

/* *******************************************************************************************
 * ghostThread, used for the ghost thread
 * void *arg, commandline args (used for building)
 ********************************************************************************************/
void *ghostThread(void *arg) {
    srand(time(NULL));

    GhostType *ghostPtr = (GhostType*) arg;
    // Repeat some events while the ghost is not bored
    while(ghostPtr->boredomTimer > 0) {
        sleep(ghostPtr->sleepTime);
        // The ghost and hunter appear in the same room
        if(checkGhostInRoom(ghostPtr)){
            // The ghost can either do nothing or add some random evidence
            int pickAction = randInt(0,2);
            // Reset the boredom timer to BOREDOM_MAX
            ghostPtr->boredomTimer = BOREDOM_MAX;
            // If the action is not 0, so 1, add some random evidence
            if(pickAction) {
                addRandomEvidence(ghostPtr);
            }
        // If the ghost does not exist in the same room
        }else{
            // The ghost can either move, addRandomEvidence, or do Nothing
            int pickAction2 = randInt(0,3);
            // Decrease bordom
            ghostPtr->boredomTimer--;
            // Pick an action
            switch(pickAction2) {
                //Move to another room
                case 0:
                    //Make similar to hunters
                    moveGhost(ghostPtr);
                    break;
                // leave evidence
                case 1: 
                    
                    addRandomEvidence(ghostPtr);
                    break;
                // Do Nothing
                case 2:
                    break;
            }
        }
    }

    return NULL;
}


int getRandomEvidenceForGhost(GhostClassType ghostType){
    int ghostPtrArr[3];
    // Creates an array containing three integers which depend on what the possible evidence types are for the given ghost
    switch(ghostType){
        case POLTERGEIST:
            {
                ghostPtrArr[0] = 0;
                ghostPtrArr[1] = 1;
                ghostPtrArr[2] = 2;
            }
            break;
        case BANSHEE:
            {
                ghostPtrArr[0] = 0;
                ghostPtrArr[1] = 1;
                ghostPtrArr[2] = 3;
            }
            break;
        case BULLIES:
            {
                ghostPtrArr[0] = 0;
                ghostPtrArr[1] = 2;
                ghostPtrArr[2] = 3;
            }
            break;
        case PHANTOM:
            {
                ghostPtrArr[0] = 1;
                ghostPtrArr[1] = 2;
                ghostPtrArr[2] = 3;
            }
            break;
    }
    // Return a random value from the newly created array
    return ghostPtrArr[randInt(0,3)];
}

/* *******************************************************************************************
 * addRandomEvidence, creates and adds some random evidence to the ghost's room
 * GhostType *currentGhost (in/out) used to add the randomEvidence to the ghost's room
 ********************************************************************************************/
void addRandomEvidence(GhostType *currentGhost) {
    sem_wait(&(currentGhost->room->mutex));

    // Allocate the node and it's evidence
    EvidenceNodeType *evidenceNode = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
    EvidenceType *newEvidencePtr = (EvidenceType*) malloc(sizeof(EvidenceType));

    // Pick a random evidenceCategory
    int randomEvidence = (getRandomEvidenceForGhost(currentGhost->ghostType));
    // printf("Evidence generated: %s\n", evidenceTypeToString(randomEvidence))

    newEvidencePtr->evidenceCategory = (EvidenceClassType) randomEvidence;
    
    // Pick some reading data for the evidence
    newEvidencePtr->readingData = generateGhostlyValueOnType(newEvidencePtr->evidenceCategory);

    // Set the node's data and next
    evidenceNode->data = newEvidencePtr;
    evidenceNode->next = NULL;
    
	addEvidenceToRoom(currentGhost->room->evidenceList, evidenceNode);
    printf("Ghost added %s %f to %s\n", evidenceTypeToString(evidenceNode->data->evidenceCategory), evidenceNode->data->readingData, currentGhost->room->name);

    sem_post(&(currentGhost->room->mutex));
}

/* *******************************************************************************************
 * addEvidenceToRoom, creates and adds some random evidence to the ghost's room
 * GhostEvidenceListType *list (out) the list to append the evidence to
 * EvidenceNodeType* evidenceNode (in) the evidence to append to the list
 ********************************************************************************************/
void addEvidenceToRoom(GhostEvidenceListType *list, EvidenceNodeType* evidenceNode){
    evidenceNode->next = NULL;
    if(list->head == NULL){ // If list is empty
        list->head = evidenceNode;
        list->tail = evidenceNode;
    }else{  // If list has at least one element
        // Add to the end
        list->tail->next = evidenceNode;
        list->tail = evidenceNode;
    }
}

/* *******************************************************************************************
 * generateValueOnType, given an evidenceType get a random float for the appropriate values
 * EvidenceClassType evidenceType (in), used to get the correct evidence value
 ********************************************************************************************/
float generateGhostlyValueOnType(EvidenceClassType evidenceType){   
    int enumAsInt = evidenceType;
    // printf("enum as int %d\n", enumAsInt);
    switch (enumAsInt){
        case 0:
            return randFloat(4.7, 5.0);
            break;
        case 1:
            return randFloat(-10.0, 1.0);
            break;
        case 2:
            return 1;
             break;
        case 3:
            return randFloat(65.0, 75.0);
            break;
    }

    return C_MISC_ERROR;
}

/* *******************************************************************************************
 * moveGhost, given an evidenceType get a random float for the appropriate values
 * GhostType *currentGhost (in/out) used to move the currentGhost to an adjacent room
 ********************************************************************************************/
void moveGhost(GhostType *currentGhost){
    // Gets the size of the adjacent rooms
    RoomNodeType *traverseRoomNode = currentGhost->room->connectedRooms->head;
    int sizeCounter = 0;
    while(traverseRoomNode != NULL){
        sizeCounter++;
        traverseRoomNode = traverseRoomNode->next;
    }
    
    // Picks a random integer between 0 and the size of the adjacent rooms
    int randomNodeInt = randInt(0,sizeCounter);
    RoomNodeType *tempRoom = currentGhost->room->connectedRooms->head;
    for(int i = 0; i < randomNodeInt; i++) {
        tempRoom = tempRoom->next;
    }

    printf("Ghost move from %s to %s\n", currentGhost->room->name, tempRoom->data->name);
    // Sets previous room's ghost to NULL
    currentGhost->room->ghost = NULL;
    // Updates ghosts room
    currentGhost->room = tempRoom->data;
    // Updates the room of the ghost
    currentGhost->room->ghost = currentGhost;
}

/* *******************************************************************************************
 * checkGhostInRoom, given a ghost, check if there are hunters in its room
 * GhostType *currentGhost (in), used to get the room data
 ********************************************************************************************/
int checkGhostInRoom(GhostType *currentGhost){
    return (currentGhost->room->hunters->size > 0) ? C_TRUE : C_FALSE;
}

/* *******************************************************************************************
 * ghostTypeToString, given a ghostType get the String that matches the ENUM
 * GhostType *currentGhost (in), used to get the ghost data
 ********************************************************************************************/
char* ghostTypeToString(GhostClassType ghost){
    switch(ghost){
        case POLTERGEIST:
            return "POLTERGEIST";
            break;
        case BANSHEE:
            return "BANSHEE";
            break;
        case BULLIES:
            return "BULLIES";
            break;
        case PHANTOM:
            return "PHANTOM";
            break;
        default:
            return "UNKNOWN";
            break;
    }
}

/* *******************************************************************************************
 * printGhost, given a ghost print its data
 * GhostType *ghost (in), used to get the ghost data
 ********************************************************************************************/
void printGhost(GhostType *ghost){
    printf("\nPRINTING GHOST\n");
    printf("Ghost Type: %s", ghostTypeToString(ghost->ghostType));
    printf("\n");
    printf("Room: %s\n", ghost->room->name);
    printf("Boredom Timer: %d\n", ghost->boredomTimer);
}

/* *******************************************************************************************
 * printGhostEvidenceList, given a ghostEvidenceList print its evidence
 * GhostEvidenceListType *ghostEvidenceList (in) used to get the evidence
 ********************************************************************************************/
void printGhostEvidenceList(GhostEvidenceListType *ghostEvidenceList, char* indents){
    EvidenceNodeType *tempEvidenceNode = ghostEvidenceList->head;
    while(tempEvidenceNode != NULL){
        printf("%sType: %s", indents, evidenceTypeToString(tempEvidenceNode->data->evidenceCategory));
        printf("\t%f %s\n", tempEvidenceNode->data->readingData, isGhostly(tempEvidenceNode->data) ? "(Ghostly)" : "");
        tempEvidenceNode = tempEvidenceNode->next;
    }
}

/* *******************************************************************************************
 * freeGhost, given a GhostType, free its evidence
 * GhostType *ghost (in) is the ghost to be freed
 ********************************************************************************************/
void freeGhost(GhostType *ghost) {
    freeEvidenceList(ghost->room->evidenceList);
    free(ghost);
}
