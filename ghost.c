#include "defs.h"

void initGhost(GhostClassType *ghostType, RoomType *room, GhostType *ghost) {
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
    printf("Room Name: %s", ghostPtr->room->name);

    
    //If the ghost is the room with a hunter reset bordin timer to boredom_max and it cannot move
    while(ghostPtr->boredomTimer > 0) {
        //Ghost is in a room with a hunter
        if(checkGhostInRoom(ghostPtr)){
            int pickAction = randInt(0,2);
            printf("In Room: %d\n", ghostPtr->boredomTimer);
            ghostPtr->boredomTimer = BOREDOM_MAX;
            if(pickAction) {
                addRandomEvidence(ghostPtr);
            }
        //Ghost is not in a room with a hunter
        }else{
            int pickAction2 = randInt(0,3);
            printf("NOT In Room: %d\n", ghostPtr->boredomTimer);
            //Decrease bordom
            ghostPtr->boredomTimer--;
            //Pick an action
            switch(pickAction2) {
                //Move to another room
                case 0:
                    //Make similar to hunters
                    int move = randomAdjacentRoom(ghostPtr);
                    if(move > 0) {
                        moveGhost(ghostPtr, move);
                    }
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
}

/*
    Makes a new evidence type for the randomEvidence
*/
void addRandomEvidence(GhostType *currentGhost) {
    //Generate new evidence
    EvidenceNodeType *evidenceNode = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
    
    EvidenceType newEvidence;
    EvidenceType *newEvidencePtr = &newEvidence;
    int randomEvidence = randInt(0, 4);
    newEvidencePtr->evidenceCategory = (EvidenceClassType) randomEvidence;
    //Probably change later, def shouldnt be this
    newEvidencePtr->readingData = generateValueOnType(newEvidencePtr->evidenceCategory);

    evidenceNode->data = newEvidencePtr;
    evidenceNode->next = NULL;
    
    //Set the evidenceList to new evidence
    if(currentGhost->room->evidenceList->head == NULL){
        currentGhost->room->evidenceList->head = evidenceNode;
        currentGhost->room->evidenceList->tail = evidenceNode;
    }else if (currentGhost->room->evidenceList->head == currentGhost->room->evidenceList->tail){
        currentGhost->room->evidenceList->head->next = evidenceNode;
        currentGhost->room->evidenceList->tail = evidenceNode;
    }else{
        currentGhost->room->evidenceList->tail->next = evidenceNode;
        currentGhost->room->evidenceList->tail = evidenceNode;
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
void moveGhost(GhostType *currentGhost, int move){
    //Move to the correct node in the linked list
    RoomNodeType *tempRoom = currentGhost->room->connectedRooms->head;
    for(int i = 0; i < move; i++) {
        tempRoom = tempRoom->next;
    }

    //Updates ghosts room
    currentGhost->room = tempRoom->data;
    //Updates the room of the ghost
    currentGhost->room->ghost = currentGhost;
}

/*
    Gets the size of the connectedRooms from the current room linkedlist, then returns a random number between 0 and length
    If the room is null return 0
*/
int randomAdjacentRoom(GhostType *currentGhost){
    //Set tempNode to head
    RoomNodeType *tempRoomNode = currentGhost->room->connectedRooms->head;

    //Set the size to 0
    int sizeCounter = 0;
    
    //Check if there is no room associated with the ghost
    if(currentGhost->room == NULL){
        return C_NO_ROOM_ERROR;
    }
    
    //Loop through the linkedlist incrementing the sizeCounter
    while(tempRoomNode != NULL){
        tempRoomNode = tempRoomNode->next;
        sizeCounter++;
    }

    //Return a random integer from 0, (sizeCounter-1)
    return randInt(0, sizeCounter);
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