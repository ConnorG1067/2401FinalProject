#include "defs.h"

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

    hunterPtr->personalEvidence = evidenceListPtr;
    
    // Initialize fear and boredom timer to initial values
    hunterPtr->fear = 0;
    hunterPtr->timer = BOREDOM_MAX;
    *hunter = hunterPtr; 
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
        printf("Hunters Choice: %d\n", hunterChoice);
        switch(hunterChoice){
            case 0:
                //Collect Evidence
                sem_wait(&(hThreadHunter->room->mutex));
                collectEvidence(hThreadHunter);
                sem_post(&(hThreadHunter->room->mutex));
                break;
            
            case 1:
                //Move
                moveHunter(hThreadHunter);
                
                break;
            case 2:
                //Communicate
                if(hThreadHunter->room->hunters->size > 1){
                    communicateEvidence(hThreadHunter);
                }
                break;
        }
    }
    return NULL;
}   

// I was under the impression that each room had a ghost, 
// though that seems to not be the case.
// This function checks whether the room that the hunter is in has a ghost in it or not
int checkHunterWithGhost(HunterType *currentHunter) { 
    if(currentHunter->room->ghost != NULL) {
        printf("%s is in a room with the ghost\n", currentHunter->name);
        return C_TRUE;
    }
    return C_FALSE;
}

int containsThreeEvidence(HunterType *currentHunter) {
    int evidenceCounter = 0;
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
        if(evidenceCounter >= 3) {
            return C_TRUE;
        }
    }
    return C_FALSE;
}


//Pretty gnarly, might break up into multiple functions and verify it works
int communicateEvidence(HunterType *currentHunter) {
    printf("\n\nCommunicate Evidence\n");

    printf("Current Hunters Evidence To Start:\n");
    EvidenceNodeType *currEvHead = currentHunter->personalEvidence->head;
    while(currEvHead != NULL){
        printf("  %s %f %s\n", evidenceTypeToString(currEvHead->data->evidenceCategory), currEvHead->data->readingData,(isGhostly(currEvHead->data)) ? "IS GHOSTLY" : "");
        currEvHead = currEvHead->next;
    }
    //Grab the room to hunter is in
    RoomType *currentRoom = currentHunter->room;
    
    int randomHunter = randInt(0, currentRoom->hunters->size);

    //Make some variables for easy calls
    //Hunter to commuinicate to
    while(currentHunter == currentRoom->hunters->hunterList[randomHunter]){
        randomHunter = randInt(0, currentRoom->hunters->size);
    }

    HunterType *hunterToCommunicate = currentRoom->hunters->hunterList[randomHunter];

    printf("To Communicate hunter evidence To start:\n");
    EvidenceNodeType *toCommunicateEv = currentRoom->hunters->hunterList[randomHunter]->personalEvidence->head;
    while(toCommunicateEv != NULL){
        printf("  %s %f %s\n", evidenceTypeToString(toCommunicateEv->data->evidenceCategory), toCommunicateEv->data->readingData, (isGhostly(toCommunicateEv->data)) ? "IS GHOSTLY" : "");
        toCommunicateEv = toCommunicateEv->next;
    }
    //The temp node of the hunter that we are communicating to
    EvidenceNodeType *hunterTraverseNode = hunterToCommunicate->personalEvidence->head;
    //The tempNode of the current hunter

    GhostEvidenceListType *evidenceCopy = makeACopyOfPersonalEvidence(currentHunter->personalEvidence);
    EvidenceNodeType *currentHunterTraverseNode = evidenceCopy->head;
    //Hunter in room --> currentHunter
    while(hunterTraverseNode != NULL) {
        //if ghostly
        if(isGhostly(hunterTraverseNode->data) && !checkIfDuplicate(currentHunter->personalEvidence, hunterTraverseNode)) {
            //make a new node
            EvidenceNodeType *deepCopyEvNode = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
            //Might share the same data cuz they're pointers
            //Future problem: make a copy room function
            //Set the currentDeepCopyNode's data to the hunter's tempNode data
            deepCopyEvNode->data = hunterTraverseNode->data;
            deepCopyEvNode->next = NULL;

            addEvidenceToHunter(currentHunter->personalEvidence, deepCopyEvNode);
        }
        hunterTraverseNode = hunterTraverseNode->next;
    }
    
 
    //currentHunter --> Hunter in room 
    while(currentHunterTraverseNode != NULL){
        if(isGhostly(currentHunterTraverseNode->data) && !checkIfDuplicate(hunterToCommunicate->personalEvidence, currentHunterTraverseNode)){
            
            addEvidenceToHunter(hunterToCommunicate->personalEvidence, currentHunterTraverseNode);
        }
        currentHunterTraverseNode = currentHunterTraverseNode->next;
    }

    printf("Current Hunters Evidence To End:\n");
    currEvHead = currentHunter->personalEvidence->head;
    while(currEvHead != NULL){
        printf("  %s %f %s\n", evidenceTypeToString(currEvHead->data->evidenceCategory), currEvHead->data->readingData,(isGhostly(currEvHead->data)) ? "IS GHOSTLY" : "");
        currEvHead = currEvHead->next;
    }

    printf("To Communicate hunter evidence To End:\n");
    toCommunicateEv = currentRoom->hunters->hunterList[randomHunter]->personalEvidence->head;
    while(toCommunicateEv != NULL){
        printf("  %s %f %s\n", evidenceTypeToString(toCommunicateEv->data->evidenceCategory), toCommunicateEv->data->readingData, (isGhostly(toCommunicateEv->data)) ? "IS GHOSTLY" : "");
        toCommunicateEv = toCommunicateEv->next;
    }
    return C_TRUE;
}

int moveHunter(HunterType* currentHunter){
    printf("\n\nMOVING\n");
    RoomType *savedPreviousRoom = currentHunter->room;
    //Find the size of the room, so we can pick a random room
    RoomNodeType *traverseRoomNode = currentHunter->room->connectedRooms->head;
    int sizeCounter = 0;
    while(traverseRoomNode != NULL){
        sizeCounter++;
        traverseRoomNode = traverseRoomNode->next;
    }

    //Pick a random number between 0 and (sizeCounter-1) (inclusive)
    int randomRoomInt = randInt(0, sizeCounter);
    //Loop a node until the for loop terminates
    RoomNodeType *randomRoomNode = currentHunter->room->connectedRooms->head;
    for(int i = 0; i<randomRoomInt; i++){
        randomRoomNode = randomRoomNode->next;
    }
    
    printf("ORIGINAL: Hunters from %s\n", currentHunter->room->name);
    for(int i = 0; i<currentHunter->room->hunters->size; i++){
        printf("%s\n", currentHunter->room->hunters->hunterList[i]->name);
    }

    printf("Moved %s from %s to %s\n", currentHunter->name, currentHunter->room->name, randomRoomNode->data->name);

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
    
    currentHunter->room->hunters->size--;

    printf("REMOVED: Hunters from %s\n", currentHunter->room->name);
    for(int i = 0; i<currentHunter->room->hunters->size; i++){
        printf("%s\n", currentHunter->room->hunters->hunterList[i]->name);
    }



    //Add him to the new room
    addHunterToRoom(randomRoomNode->data, currentHunter);
    currentHunter->room = randomRoomNode->data;

    currentHunter->timer--;

    

    printf("NEW ROOM: Hunters from %s\n", randomRoomNode->data->name);
    for(int i = 0; i<currentHunter->room->hunters->size; i++){
        printf("%s\n", currentHunter->room->hunters->hunterList[i]->name);
    }  

    //PRINT TO VERIFY FUNCTION IS FUNCTIONAL
    printf("\n\n");
    return C_TRUE;
}

// Checks all rooms' evidence collection
//  If there is evidence that matches the type of evidence the hunter can detect
//   remove it from the evidence collection
//   Add it to the hunter's evidence collection
void collectEvidence(HunterType *currentHunter) {
    printf("\n\nORIGINAL: Room's Evidence List\n");
    EvidenceNodeType *newTempEvidence = currentHunter->room->evidenceList->head;


    while(newTempEvidence != NULL){
        printf("Category: %s\n", evidenceTypeToString(newTempEvidence->data->evidenceCategory));
        newTempEvidence = newTempEvidence->next;
    }

    if(currentHunter->room->evidenceList->head == NULL) {
        // give him random standard evidence of his tool type
        // MAKE INIT FUNCTION
        EvidenceNodeType *newEvidence = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
        newEvidence->data = (EvidenceType*) malloc(sizeof(EvidenceType));
        newEvidence->data->evidenceCategory = currentHunter->evidence;
        newEvidence->data->readingData = generateStandardValue(newEvidence->data->evidenceCategory);
        newEvidence->next = NULL;
        // Create newEvidence, give it a standard value
        addEvidenceToHunter(currentHunter->personalEvidence, newEvidence);

        printf("STANDARD EV:(no ev) added evidence: %s\n", evidenceTypeToString(currentHunter->personalEvidence->tail->data->evidenceCategory));
        return;
    }

    EvidenceNodeType *tempEvidence = currentHunter->room->evidenceList->head;
    while(tempEvidence != NULL) {
        if(tempEvidence->data->evidenceCategory == currentHunter->evidence) {
            //   remove it from the evidence collection
            removeEvidenceFromRoom(currentHunter->room->evidenceList, tempEvidence);
            
            printf("Hunters evidence List BEFORE\n");
            EvidenceNodeType *temptemptemp = currentHunter->personalEvidence->head;
            while(temptemptemp != NULL){
                printf("Type in while: %s\n", evidenceTypeToString(temptemptemp->data->evidenceCategory));
                temptemptemp = temptemptemp->next;
            }

            addEvidenceToHunter(currentHunter->personalEvidence, tempEvidence);

            printf("Hunters evidence List AFTER\n");
            EvidenceNodeType *temptemp = currentHunter->personalEvidence->head;
            while(temptemp != NULL){
                printf("Type in while: %s\n", evidenceTypeToString(temptemp->data->evidenceCategory));
                temptemp = temptemp->next;
            }
            //Reset the hunter boredom timer if evidence detected is ghostly
            if(isGhostly(tempEvidence->data)) {
                currentHunter->timer = BOREDOM_MAX;
                printf("IT IS GHOSTLY: %d\n", currentHunter->timer);
            }

            printf("DETECTED EV: added evidence: %s\n", evidenceTypeToString(currentHunter->personalEvidence->tail->data->evidenceCategory));
            printf("REMOVE: Room's Evidence List\n");

            printf("NEW: Room's Evidence List\n");
            EvidenceNodeType *newTempEvidence = currentHunter->room->evidenceList->head;
            while(newTempEvidence != NULL){
                printf("Type in while: %s\n", evidenceTypeToString(newTempEvidence->data->evidenceCategory));
                newTempEvidence = newTempEvidence->next;
            }
        }
        tempEvidence = tempEvidence->next;

    }
    printf("\n\n\n");
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
    //If we want to remove the head
    if(evidence == tempEvidence){
        list->head = list->head->next;
        //Free previous head
    }

    while(tempEvidence->next != NULL) {
        if(tempEvidence->next == evidence){
            //Prob need to free unless hunter uses it
            tempEvidence->next = tempEvidence->next->next;
            return;
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
    evidence->next = NULL;

	if(list->head == NULL){
		list->head = evidence;
		list->tail = evidence;
	}else{
		list->tail->next = evidence;
		list->tail = evidence;
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

char* evidenceTypeToString(EvidenceClassType evidence){
    switch(evidence){
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

void printEvidence(EvidenceType *evidence){
    printf("\nPRINTING EVIDENCE\n");
    printf("Type: %s", evidenceTypeToString(evidence->evidenceCategory));
    printf("Reading Data: %f", evidence->readingData);
    
}

void printHunter(HunterType* hunter){
    printf("\nHunter Info\n");
    printf("Name: %s\n", hunter->name);
    printf("Fear Timer: %d\n", hunter->fear);
    printf("Boredom Timer: %d\n", hunter->timer);
    printf("Evidence Type: %s\n", evidenceTypeToString(hunter->evidence));
    printf("Hunter's Room: %s\n", hunter->room->name);
    // printRoom(hunter->room);
    // printGhostEvidenceList(hunter->personalEvidence);
}


void printHunterList(HunterListType *list) {
    printf("Hunters\n");
    for(int i = 0; i < list->size; i++){
        printf("  Name: %s\n", (list->hunterList[i]->name));
    }
}


GhostEvidenceListType* makeACopyOfPersonalEvidence(GhostEvidenceListType *toCopyList){
    //Make A list and init
    GhostEvidenceListType *copyListPtr = (GhostEvidenceListType*) malloc(sizeof(GhostEvidenceListType));
    initGhostList(copyListPtr);

    EvidenceNodeType *traverseNode = toCopyList->head;
    while(traverseNode != NULL){
        EvidenceNodeType *tempEvNode = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
        tempEvNode->data = traverseNode->data;
        addEvidenceToHunter(copyListPtr, tempEvNode);
        traverseNode = traverseNode->next;
    }

    return copyListPtr;
}

int checkIfDuplicate(GhostEvidenceListType *list, EvidenceNodeType* node){
    EvidenceNodeType *traverseNode = list->head;
    while(traverseNode != NULL){
        if(traverseNode->data->evidenceCategory == node->data->evidenceCategory && traverseNode->data->readingData == node->data->readingData){
            return C_TRUE;
        }
        traverseNode = traverseNode->next;
    }

    return C_FALSE;
}