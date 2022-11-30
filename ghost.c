#include "defs.h"

/* *******************************************************************************************
 * initGhost, initalizes the given ghost
 * GhostClassType (in) the type of ghost
 * RoomType *room (in) the room the ghost exists in
 * GhostType *ghost (out), the ghost pointer to set for each initalization
 ********************************************************************************************/
void initGhost(GhostClassType ghostType, RoomType *room, GhostType *ghost) {
    ghost->ghostType = ghostType;
    ghost->room = room;
    ghost->boredomTimer = BOREDOM_MAX;
}

/* *******************************************************************************************
 * initGhostList, initalizes the given ghost evidence list
 * GhostEvidenceListType *ghostEvidenceList (out), used to set the head & tail to NULL
 ********************************************************************************************/
void initGhostList(GhostEvidenceListType *ghostEvidenceList){
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

    BuildingType *gThreadBuilding = (BuildingType*) arg;

    // Allocate and Initalize a new ghost
    GhostType *ghostPtr = (GhostType*) malloc(sizeof(GhostType));
    
    // NOTE HERE: Pick a random ghost, DO NOT leave it as POLTERGEIST
    initGhost(POLTERGEIST, getRandomRoom(gThreadBuilding->rooms->head)->data, ghostPtr);

    
    // Repeat some events while the ghost is not bored
    while(ghostPtr->boredomTimer > 0) {
        printf("Ghost Boredom Timer: %d\n", ghostPtr->boredomTimer);
        // The ghost and hunter appear in the same room
        if(checkGhostInRoom(ghostPtr)){
            // The ghost can either do nothing or add some random evidence
            int pickAction = randInt(0,2);
            // Reset the boredom timer to BOREDOM_MAX
            ghostPtr->boredomTimer = BOREDOM_MAX;

            // If the action is not 0, so 1, add some random evidence
            if(pickAction) {
                printf("GHOST ADDED EVIDENCE & IN SAME ROOM\n");
                addRandomEvidence(ghostPtr);
            }
        // If the ghost does not exist in the same room
        }else{
            // The ghost can either move, addRandomEvidence, or do Nothing
            int pickAction2 = randInt(0,3);
            //Decrease bordom
            ghostPtr->boredomTimer--;
            //Pick an action
            switch(pickAction2) {
                //Move to another room
                case 0:
                    //Make similar to hunters
                    printf("\n\nMOVING GHOST\n");
                    moveGhost(ghostPtr);
                    break;
                // leave evidence
                case 1: 
                    printf("\n\nAdding Evidence\n");
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

/* *******************************************************************************************
 * addRandomEvidence, creates and adds some random evidence to the ghost's room
 * GhostType *currentGhost (in/out) used to add the randomEvidence to the ghost's room
 ********************************************************************************************/
void addRandomEvidence(GhostType *currentGhost) {
    
    // Allocate the node and it's evidence
    EvidenceNodeType *evidenceNode = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
    EvidenceType *newEvidencePtr = (EvidenceType*) malloc(sizeof(EvidenceType));

    // Pick a random evidenceCategory
    int randomEvidence = randInt(0, 4);
    newEvidencePtr->evidenceCategory = (EvidenceClassType) randomEvidence;
    
    // Pick some reading data for the evidence
    newEvidencePtr->readingData = generateValueOnType(newEvidencePtr->evidenceCategory);

    // Set the node's data and next
    evidenceNode->data = newEvidencePtr;
    evidenceNode->next = NULL;
    
    // PRINTING BULLSHIT
    printf("WHAT WE WANNA ADD\n");
    printf("Name: %s\n", evidenceTypeToString(evidenceNode->data->evidenceCategory));
    printf("Reading: %f\n", evidenceNode->data->readingData);

    // Appends the evidence node to the room's evidence list
	addEvidenceToRoom(currentGhost->room->evidenceList, evidenceNode);
    
    // PRINTING BULLSHIT
    printf("PRINTING THE ENTIRE EVIDENCE LIST\n");
    EvidenceNodeType *evNodeTemp = currentGhost->room->evidenceList->head;
    while(evNodeTemp != NULL){
        printf("Name: %s\n", evidenceTypeToString(evNodeTemp->data->evidenceCategory));
        printf("Reading: %f\n", evNodeTemp->data->readingData);
        evNodeTemp = evNodeTemp->next;
    }
}

/* *******************************************************************************************
 * addEvidenceToRoom, creates and adds some random evidence to the ghost's room
 * GhostEvidenceListType *list (out) the list to append the evidence to
 * EvidenceNodeType* evidenceNode (in) the evidence to append to the list
 ********************************************************************************************/
void addEvidenceToRoom(GhostEvidenceListType *list, EvidenceNodeType* evidenceNode){
    if(list->head == NULL){
		list->head = evidenceNode;
		list->tail = evidenceNode;
	}else{
		list->tail->next = evidenceNode;
		list->tail = evidenceNode;
    }
}

/* *******************************************************************************************
 * generateValueOnType, given an evidenceType get a random float for the appropriate values
 * EvidenceClassType evidenceType (in), used to get the correct evidence value
 ********************************************************************************************/
int generateValueOnType(EvidenceClassType evidenceType){   
    int enumAsInt = evidenceType;
    switch (enumAsInt){
        case 0:
            return randFloat(0, 5);
            break;
        case 1:
            return randFloat(-10, 27);
            break;
        case 2:
            return randFloat(0, 1);
             break;
        case 3:
            return randFloat(40, 75);
            break;
    }

    return C_MISC_ERROR;
}

/* *******************************************************************************************
 * moveGhost, given an evidenceType get a random float for the appropriate values
 * GhostType *currentGhost (in/out) used to move the currentGhost to an adjacent room
 ********************************************************************************************/
void moveGhost(GhostType *currentGhost){
    // PRINTING BULLSHIT
    printf("CURRENT: ghost current room %s\n", currentGhost->room->name);

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


    // Sets previous room's ghost to NULL
    currentGhost->room->ghost = NULL;
    // Updates ghosts room
    currentGhost->room = tempRoom->data;
    // Updates the room of the ghost
    currentGhost->room->ghost = currentGhost;

    // PRINTING BULLSHIT
    printf("AFTER: ghost NEW room %s\n", currentGhost->room->name);
}


/* *******************************************************************************************
 * checkGhostInRoom, given a ghost, check if there are hunters in its room
 * GhostType *currentGhost (in), used to get the room data
 ********************************************************************************************/
int checkGhostInRoom(GhostType *currentGhost){
    // If the size of the hunters in the given room, in which the ghost resides, is greater than 0, return C_TRUE
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
void printGhostEvidenceList(GhostEvidenceListType *ghostEvidenceList){
    EvidenceNodeType *tempEvidenceNode = ghostEvidenceList->head;
    while(tempEvidenceNode != NULL){
        printEvidence(tempEvidenceNode->data);
        tempEvidenceNode = tempEvidenceNode->next;
    }
}