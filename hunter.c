#include "defs.h"


// void initGhost(int id, GhostEnumType gt, RoomType *r, float like, GhostType **ghost){
// 	//Allocate memory for the ghost	
// 	GhostType *ghostPointer = malloc(sizeof(GhostType));

// 	//Set Fields
// 	ghostPointer->id = id;
// 	ghostPointer->ghostType = gt;
// 	ghostPointer->room = r;
// 	ghostPointer->likelihood = like;

// 	//Set the pointer to the new ghost pointer
// 	*ghost = ghostPointer;
	
// }
void initHunter(char* name, RoomType *room, HunterType **hunter) {
    //Iit name
    HunterType *hunterPtr = (HunterType*) malloc(sizeof(HunterType));

    strcpy(hunterPtr->name, name);

    //Give them a random evidence tool
    int randomEvidence = randInt(0, 4);
    hunterPtr->evidence = (EvidenceClassType) randomEvidence;

    hunterPtr->room = room;

    //Init personal evidence
    GhostEvidenceListType *evidenceListPtr = (GhostEvidenceListType*) malloc(sizeof(GhostEvidenceListType));
    initPersonalEvidence(evidenceListPtr);
    hunterPtr->personalEvidence;
    if(hunterPtr->personalEvidence->head == NULL){
        printf("YO");
    }
    
    // Initialize fear and boredom timer to initial values
    hunterPtr->fear = 0;
    hunterPtr->timer = BOREDOM_MAX;
}  

void initPersonalEvidence(GhostEvidenceListType* evidenceList){
    evidenceList->head = NULL;
    evidenceList->tail = NULL;
}

void initHunterList(HunterListType *hunters) {
    hunters->size = 0;
}

int addHunterToList(HunterListType* hunters, HunterType* hunter){
    if(hunters->size<MAX_HUNTERS){
        hunters->hunterList[hunters->size++] = hunter;
        return C_TRUE;
    }else{
        return C_FALSE;
    }
}

void *hunterThread(void *arg) {


    HunterType *hThreadHunter = (HunterType*) arg;
    // Exit if there are three different pieces of evidence
    //  OR fear level is greater than or equal to 100
    //  OR if boredom timer is <= 0
    // Exit the while loop, thus exiting the thread.
    while(!(containsThreeEvidence(hThreadHunter) || (hThreadHunter->fear >= 100) || (hThreadHunter->timer <= 0))) {
        if(checkHunterWithGhost(hThreadHunter)) {
            hThreadHunter->fear++;
            hThreadHunter->timer = BOREDOM_MAX;
        }

        int hunterChoice = randInt(0,3);

        switch(hunterChoice){
            case 0:
                //Collect Evidence
                collectEvidence(hThreadHunter);
                break;
            
            case 1:
                //Move
                moveHunter(hThreadHunter);
                break;
            case 2:
                //Communicate
                communicateEvidence(hThreadHunter);
                break;

        }
        // Randomly (or using our own programmed logic) either 
        //  collect evidence
        //  move
        //  or communicate evidence if the hunter is in the same room as another hunter
            
        
    }
}   

// I was under the impression that each room had a ghost, 
// though that seems to not be the case.
// This function checks whether the room that the hunter is in has a ghost in it or not
int checkHunterWithGhost(HunterType *currentHunter) { 
    if(currentHunter->room->ghost != NULL) {
        return C_TRUE;
    }
    return C_FALSE;
}

int containsThreeEvidence(HunterType *currentHunter) {

    int evidenceCounter = 0;
    printf("HERE\n");
    if(currentHunter->personalEvidence->head == NULL){
        return C_FALSE;
    }
    EvidenceNodeType *tempEvidenceNode = currentHunter->personalEvidence->head;

    if(currentHunter->personalEvidence->head != NULL) {
        evidenceCounter++;
        // Iterate over list, until you reach the end, keep track of how many elements there are
        while(tempEvidenceNode != currentHunter->personalEvidence->tail) {
            tempEvidenceNode = tempEvidenceNode->next;
            evidenceCounter++;
        }
        if(evidenceCounter == 3) {
            return C_TRUE;
        }
    }
    return C_FALSE;
}


//Pretty gnarly, might break up into multiple functions and verify it works
int communicateEvidence(HunterType *currentHunter) {
    //Grab the room to hunter is in
    RoomType *currentRoom = currentHunter->room;
    int randomHunter = randInt(0, currentRoom->hunters->size);

    //Make some variables for easy calls
    //Hunter to commuinicate to
    HunterType *hunterToCommunicate = currentRoom->hunters->hunterList[randomHunter];
    
    //The temp node of the hunter that we are communicating to
    EvidenceNodeType *hunterTraverseNode = hunterToCommunicate->personalEvidence->head;
    //The tempNode of the current hunter
    EvidenceNodeType *currentHunterTraverseNode = currentHunter->personalEvidence->head;
    //Hunter in room --> currentHunter
    while(hunterTraverseNode != NULL){

        //if ghostly
        if(isGhostly(hunterTraverseNode->data)){
            //make a new node
            EvidenceNodeType *deepCopyEvNode = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
            //Might share the same data cuz they're pointers
            //Future problem: make a copy room function
            //Set the currentDeepCopyNode's data to the hunter's tempNode data
            deepCopyEvNode->data = hunterTraverseNode->data;
            deepCopyEvNode->next = NULL;

            addEvidenceToHunter(currentHunter->personalEvidence, deepCopyEvNode);
            hunterTraverseNode = hunterTraverseNode->next;
        }
       
    }
    
    //currentHunter --> Hunter in room 
    while(currentHunterTraverseNode != NULL){
        if(isGhostly(currentHunterTraverseNode->data)){
            //Make a new node
            EvidenceNodeType *deepCopyEvNode = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
            //Might share the same data cuz they're pointers
            //Future problem: make a copy room function
            deepCopyEvNode->data = currentHunterTraverseNode->data;
            deepCopyEvNode->next = NULL;

            addEvidenceToHunter(hunterToCommunicate->personalEvidence, deepCopyEvNode);
            currentHunterTraverseNode = currentHunterTraverseNode->next;
        }
    }
    return C_TRUE;
}

int moveHunter(HunterType* currentHunter){
    //Find the size of the room, so we can pick a random room
    RoomNodeType *traverseRoomNode = currentHunter->room->connectedRooms->head;
    int sizeCounter = 0;

    //Traverse and find the size
    while(traverseRoomNode != NULL){
        sizeCounter++;
        traverseRoomNode = traverseRoomNode->next;
    }

    //Pick a random number between 0 and (sizeCounter-1) (inclusive)
    int randomRoomInt = randInt(0, sizeCounter);
    
    //Loop a node until the for loop terminates
    RoomNodeType *randomRoomNode = currentHunter->room->connectedRooms->head;
    for(int i = 0; i<randomRoomInt; i++){
        randomRoomNode->next = randomRoomNode;
    }

    //Remove the current hunter from his previous room
    int size = currentHunter->room->hunters->size;
    for(int i = 0; i<size; i++){
        if(currentHunter->room->hunters->hunterList[i] == currentHunter){
            for(int j = i+1; j<size; j++){
                currentHunter->room->hunters->hunterList[j-1] = currentHunter->room->hunters->hunterList[j];
            }
            i = size;
        }
    }

    //Add him to the new room
    addHunterToRoom(randomRoomNode->data, currentHunter);
    currentHunter->room = randomRoomNode->data;

    currentHunter->timer--;

    return C_TRUE;
}

// Checks all rooms' evidence collection
//  If there is evidence that matches the type of evidence the hunter can detect
//   remove it from the evidence collection
//   Add it to the hunter's evidence collection
void collectEvidence(HunterType *currentHunter) {
    EvidenceNodeType *tempEvidence = currentHunter->room->evidenceList->head;

    if(currentHunter->room->evidenceList->head == NULL) {
        // give him random standard evidence of his tool type
        // MAKE INIT FUNCTION
        EvidenceNodeType *newEvidence = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
        newEvidence->data->evidenceCategory = currentHunter->evidence;
        newEvidence->data->readingData = generateStandardValue(newEvidence->data->evidenceCategory);
        newEvidence->next = NULL;
        // Create newEvidence, give it a standard value
        addEvidenceToHunter(currentHunter->personalEvidence, newEvidence);
        return;
    }

    while(tempEvidence != NULL) {
        if(tempEvidence->data->evidenceCategory == currentHunter->evidence) {
            //   remove it from the evidence collection
            removeEvidenceFromRoom(currentHunter->room->evidenceList, tempEvidence);
            addEvidenceToHunter(currentHunter->personalEvidence, tempEvidence);

            //Reset the hunter boredom timer if evidence detected is ghostly
            if(isGhostly(tempEvidence->data)) {
                currentHunter->timer = BOREDOM_MAX;
            }
        }
    }
    return;
}

int generateStandardValue(EvidenceClassType evidenceClass){
    switch(evidenceClass) { 
        case 0:
            return randFloat(0, 4.90);
            break;
        case 1:
            return randFloat(0, 27.00);
            break;
        case 2:
            return 0;
            break;
        case 3:
            return randFloat(40.0, 70.0);
            break;
    }
    return C_MISC_ERROR;
}



/*
    Removes evidence from a room
*/
void removeEvidenceFromRoom(GhostEvidenceListType *list, EvidenceNodeType *evidence) {
    EvidenceNodeType *tempEvidence = list->head;
    while(tempEvidence->next != NULL) {
        if(tempEvidence->next == evidence){
            //Prob need to free unless hunter uses it
            tempEvidence->next = tempEvidence->next->next;
        }
        tempEvidence = tempEvidence->next;
    }

    // // ALSO ACCOUNT FOR CASES WHEN NODE TO DELETE IS THE HEAD AND WHEN IT'S THE TAIL
    // if(tempEvidence != evidence) {
    //     //Return some error code
    //     printf("Evidence not found whilst attempting to remove.\n");
    // } else {
    //     //Probably free later
    //     //free(tempEvidence->next);
    // }

}

/*
  Function: addGhost(GhostListType *list, GhostType *ghost)
  Purpose:  Add a ghost to a GhostListType
       in:  ghost - the ghost we are adding
       GhostListType:  list - the list we are adding the ghost to
*/ 
void addEvidenceToHunter(GhostEvidenceListType *list, EvidenceNodeType *evidence) {
//   EvidenceNodeType *newEvidence = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
//   newEvidence->data = ;
//   newEvidence->next = NULL;

  if(list->head == NULL) {  // If list empty
    list->head = evidence;
    list->tail = evidence;
  } else if(list->head == list->tail){
    list->tail = evidence;
    list->head->next = list->tail;
  } else {  // If list has more than 1 element
    list->tail->next = evidence;
    list->tail = list->tail->next;
    list->tail->next = NULL;
  }
  return;
}


/*
    Returns a bool to determine is evidence ghostly.
*/
int isGhostly(EvidenceType *evidence){
    switch(evidence->evidenceCategory) {
        case 0:
            if(evidence->readingData >= 4.70 && evidence->readingData <= 5.00){
                return C_TRUE;
            }
            break;
        case 1:
            if(evidence->readingData >=-10 && evidence->readingData <=1){
                return C_TRUE;
            }
            break;
        case 2:
            if(evidence->readingData == 1.00) { // This might give issues since it's comparing floats
                return C_TRUE;
            }
            break;
        case 3:
            if(evidence->readingData >= 65.00 && evidence->readingData <= 75.00) {
                return C_TRUE;
            }
            break;
    }
    return C_FALSE;
}