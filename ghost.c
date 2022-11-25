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
void *ghostThread(GhostType *currentGhost) {
    int isNotBored = 1;
    int pickAction = randInt(0,2);

    //If the ghost is the room with a hunter reset bordin timer to boredom_max and it cannot move
    while(isNotBored) {
        if(!isNotBored){
            //End somehow
        }
        //Ghost is in a room with a hunter
        if(checkGhostInRoom(currentGhost)){
            currentGhost->boredomTimer = BOREDOM_MAX;
            if(pickAction) {
                addRandomEvidence(currentGhost);
            }
        //Ghost is not in a room with a hunter
        }else{
            //Decrease bordom
            currentGhost->boredomTimer--;
            //Pick an action
            switch(pickAction) {
                //Move to another room
                case 0:
                    //Make similar to hunters
                    int move = randomAdjacentRoom(currentGhost);
                    if(move > 0) {
                        moveGhost(currentGhost, move);
                    }
                    break;
                // leave evidence
                case 2: 
                    addRandomEvidence(currentGhost);
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
    EvidenceNodeType *evidenceNode;
    
    EvidenceType *newEvidence;
    int randomEvidence = randInt(0, 4);
    newEvidence->evidenceCategory = (EvidenceClassType) randomEvidence;
    //Probably change later, def shouldnt be this
    newEvidence->readingData = generateValueOnType(newEvidence->evidenceCategory);

    evidenceNode->data = newEvidence;
    evidenceNode->next = NULL;
    
    //Set the evidenceList to new evidence
    currentGhost->room->evidenceList->tail->next = evidenceNode;
    currentGhost->room->evidenceList->tail = evidenceNode;
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