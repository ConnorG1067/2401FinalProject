#include "defs.h"

void initGhost(GhostClassType ghostType, RoomType *room, GhostType *ghost) {
    ghost->ghostType = ghostType;
    ghost->room = room;
    ghost->boredomTimer = BOREDOM_MAX;
}

void initGhostList(GhostEvidenceListType *ghostEvidenceList){
    ghostEvidenceList->head = NULL;
    ghostEvidenceList->tail = NULL;
}

/*
    Main ghost thread function
*/

RoomNodeType* getRandomRoom(RoomNodeType *head){
    int randomRoomInt = randInt(1, TOTAL_ROOMS);
    RoomNodeType *traverseRoomNode = head;
    for(int i = 0; i<randomRoomInt; i++){
        traverseRoomNode = traverseRoomNode->next;
    }
    return traverseRoomNode;
}

// GhostClassType getRandomGhostType() {
//     return (GhostClassType) randInt(0,5)
// }

void *ghostThread(void *arg) {
    BuildingType *gThreadBuilding = (BuildingType*) arg;
    
    GhostType *ghostPtr = (GhostType*) malloc(sizeof(GhostType));
    initGhost(POLTERGEIST, getRandomRoom(gThreadBuilding->rooms->head)->data, ghostPtr);

    
    //If the     is the room with a hunter reset bordin timer to boredom_max and it cannot move
    while(ghostPtr->boredomTimer > 0) {
        printf("Ghost Boredom Timer: %d\n", ghostPtr->boredomTimer);
        //Ghost is in a room with a hunter
        // if(checkGhostInRoom(ghostPtr)){
        //     int pickAction = randInt(0,2);
        //     ghostPtr->boredomTimer = BOREDOM_MAX;
        //     if(pickAction) {
        //         printf("GHOST ADDED EVIDENCE & IN SAME ROOM\n");
        //         addRandomEvidence(ghostPtr);
        //     }
        //Ghost is not in a room with a hunter
        // }else{
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
            // }
        }
    }
    return NULL;
}

/*
    Makes a new evidence type for the randomEvidence
*/
void addRandomEvidence(GhostType *currentGhost) {
    //Generate new evidence
    EvidenceNodeType *evidenceNode = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
    
    EvidenceType *newEvidencePtr = (EvidenceType*) malloc(sizeof(EvidenceType));
    int randomEvidence = randInt(0, 4);
    newEvidencePtr->evidenceCategory = (EvidenceClassType) randomEvidence;
    //Probably change later, def shouldnt be this
    newEvidencePtr->readingData = generateValueOnType(newEvidencePtr->evidenceCategory);

    evidenceNode->data = newEvidencePtr;
    evidenceNode->next = NULL;
    
    printf("WHAT WE WANNA ADD\n");
    printf("Name: %s\n", evidenceTypeToString(evidenceNode->data->evidenceCategory));
    printf("Reading: %f\n", evidenceNode->data->readingData);


	
	//Setting the end of the list to the new node

    
	if(currentGhost->room->evidenceList->head == NULL){
		currentGhost->room->evidenceList->head = evidenceNode;
		currentGhost->room->evidenceList->tail = evidenceNode;
	}else{
		currentGhost->room->evidenceList->tail->next = evidenceNode;
		currentGhost->room->evidenceList->tail = evidenceNode;
    }
    
    printf("PRINTING THE ENTIRE EVIDENCE LIST\n");
    EvidenceNodeType *evNodeTemp = currentGhost->room->evidenceList->head;
    while(evNodeTemp != NULL){
        printf("Name: %s\n", evidenceTypeToString(evNodeTemp->data->evidenceCategory));
        printf("Reading: %f\n", evNodeTemp->data->readingData);
        evNodeTemp = evNodeTemp->next;
    }
    
    return;
}

/*
    Generates a random value depending on the evidence type provided
*/
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
/*
    Takes in an integer and a ghost
    Utilizes the randomAdjacentRoom function to decide on a value to move to
*/
void moveGhost(GhostType *currentGhost){

    printf("CURRENT: ghost current room %s\n", currentGhost->room->name);

    //Find the size of the room, so we can pick a random room
    RoomNodeType *traverseRoomNode = currentGhost->room->connectedRooms->head;
    int sizeCounter = 0;
    while(traverseRoomNode != NULL){
        sizeCounter++;
        traverseRoomNode = traverseRoomNode->next;
    }
    
    //Move to the correct node in the linked list
    int randomNodeInt = randInt(0,sizeCounter);
    RoomNodeType *tempRoom = currentGhost->room->connectedRooms->head;
    for(int i = 0; i < randomNodeInt; i++) {
        tempRoom = tempRoom->next;
    }


    //Sets previous room's ghost to NULL
    
    currentGhost->room->ghost = NULL;
    //Updates ghosts room
    currentGhost->room = tempRoom->data;
    //Updates the room of the ghost
    currentGhost->room->ghost = currentGhost;
    printf("AFTER: ghost NEW room %s\n", currentGhost->room->name);
}


/*
    check if the ghost happens to be in the same room as the hunter
    Return 1 if the currentGhosts room has a hunter size greater than 1
    Otherwise, return 0 false
*/
int checkGhostInRoom(GhostType *currentGhost){
    if(currentGhost->room->hunters->size > 0){
        return C_TRUE;
    }else{
        return C_FALSE;
    }
}

char* ghostTypeToString(GhostClassType ghost){
    switch(ghost){
        case EMF:
            return "EMF";
            break;
        case TEMPERATURE:
            return "TEMPERATURE";
            break;
        case FINGERPRINTS:
            return "FINGERPRINTS";
            break;
        case SOUND:
            return "SOUND";
            break;
    }
}

void printGhost(GhostType *ghost){
    printf("\nPRINTING GHOST\n");
    printf("Ghost Type: %s", ghostTypeToString(ghost->ghostType));
    printf("\n");
    printf("Room: %s\n", ghost->room->name);
    printf("Boredom Timer: %d\n", ghost->boredomTimer);
}

void printGhostEvidenceList(GhostEvidenceListType *ghostEvidenceList){
    EvidenceNodeType *tempEvidenceNode = ghostEvidenceList->head;
    while(tempEvidenceNode != NULL){
        printEvidence(tempEvidenceNode->data);
        tempEvidenceNode = tempEvidenceNode->next;
    }
}