#include "defs.h"


/* *******************************************************************************************
 * initHunter, initalizes the given hunter
 * char* name (in) the name of the given HunterType
 * RoomType *room (in) the room hunter is meant to reside in
 * HunterType **hunter (out) the hunter that the function initalizes
 ********************************************************************************************/
void initHunter(char* name, RoomType *room, HunterType **hunter) {
    // Allocate memory
    HunterType *hunterPtr = (HunterType*) malloc(sizeof(HunterType));

    // Copy the name
    strcpy(hunterPtr->name, name);

    // Generate random evidence 
    int randomEvidence = randInt(0, 4);
    hunterPtr->evidence = (EvidenceClassType) randomEvidence;

    // Assign the room
    hunterPtr->room = room;

    
    // Allocate & Initalize a personalEvidence list
    GhostEvidenceListType *evidenceListPtr = (GhostEvidenceListType*) malloc(sizeof(GhostEvidenceListType));
    initPersonalEvidence(evidenceListPtr);
    hunterPtr->personalEvidence = evidenceListPtr;
    
    // Initialize fear & boredom timer
    hunterPtr->fear = 0;
    hunterPtr->timer = BOREDOM_MAX;

    // Assign the hunter given to the hunterPtr made
    *hunter = hunterPtr; 
}  

/* *******************************************************************************************
 * initPersonalEvidence, initalizes a hunters personalEvidence list
 * GhostEvidenceListType* evidenceList (out), used to set the head and tail to NULL
 ********************************************************************************************/
void initPersonalEvidence(GhostEvidenceListType* evidenceList){
    evidenceList->head = NULL;
    evidenceList->tail = NULL;
}

/* *******************************************************************************************
 * initHunterList, initalizes a hunter list by setting the size to 0
 * HunterListType *hunters (in/out), used to set the size
 ********************************************************************************************/
void initHunterList(HunterListType *hunters) {
    hunters->size = 0;
}

/* *******************************************************************************************
 * addHunterToList, initalizes a hunter list by setting the size to 0
 * HunterListType* hunters (out) used to access the array structyre
 * HunterType* hunter (in) used to set the hunter to a specific index
 ********************************************************************************************/
int addHunterToList(HunterListType* hunters, HunterType* hunter){
    // If the size is not exceeding the MAX_HUNTERS add the hunter & return C_TRUE
    if(hunters->size<MAX_HUNTERS){
        hunters->hunterList[hunters->size++] = hunter;
        return C_TRUE;
    //Otherwise return C_FALSE
    }else{
        return C_FALSE;
    }
}

/* *******************************************************************************************
 * hunterThread, utilizes all the helper functions made to preform specific processes
 * void *arg is the commandline args which are used to access the hThreadHunter
 ********************************************************************************************/
void *hunterThread(void *arg) {
    // Set a local hunter pointer which points to the argument
    HunterType *hThreadHunter = (HunterType*) arg;
    
    // Exit if there are three different pieces of evidence
    //  OR fear level is greater than or equal to 100
    //  OR if boredom timer is <= 0
    // Exit the while loop, thus exiting the thread.
    // sem_wait(&(hThreadHunter->room->mutex));
    while(!(containsThreeEvidence(hThreadHunter) || (hThreadHunter->fear >= 100) || (hThreadHunter->timer <= 0))) {
        // printf("Main hunter thread while\n");
        // Check if the hThreadHunter and the ghost exist in the same room
        if(checkHunterWithGhost(hThreadHunter)) {
            // If they do increase the fear & reset the boredom timer
            hThreadHunter->fear++;
            hThreadHunter->timer = BOREDOM_MAX;
        }

        // Pick a random integer between 0-2 (inclusive)
        int hunterChoice = randInt(0,3);
        // printf("Hunters Choice: %d\n", hunterChoice);
        // printf("Room %s with semaphore value: %d\n", hThreadHunter->room->name, g);

        switch(hunterChoice){
            case 0:
                // Collect Evidence
                sem_wait(&(hThreadHunter->room->mutex));
                collectEvidence(hThreadHunter);
                sem_post(&(hThreadHunter->room->mutex));
                break;
            
            case 1:
                // Move
                // printf("\n\nMOVE HUTNER: \n");
                moveHunter(hThreadHunter);
                // printf("\n\n\nRELEASED MOVE\n\n\n");
                break;
            case 2:
                //Communicate
                sem_wait(&(hThreadHunter->room->mutex));
                if(hThreadHunter->room->hunters->size > 1){
                    // printf("COMMUNICATE ROOM:\n");
                    communicateEvidence(hThreadHunter);
                }
                sem_post(&(hThreadHunter->room->mutex));
                break;
        }

    }
    // printf("Boredom Timer %d\n", hThreadHunter->timer);
    // printf("Fear Timer %d\n", hThreadHunter->fear);
    // printf("contains evidence: %s\n", containsThreeEvidence(hThreadHunter) ? "True" : "False");
    // printf("REMOVING HUNTER %s\n", hThreadHunter->name);
    // printf("Contains 3 evidence: %s, fear>=100: %s, boredom<=0: %s", (containsThreeEvidence(hThreadHunter)) ? "True" : "False", (hThreadHunter->fear>=100) ? "True" : "False", (hThreadHunter->timer) ? "True" : "False");
    removeHunterFromRoom(hThreadHunter);
    // printf("ENDING HUNTER THREAD\n");
    // sem_getvalue(&(hThreadHunter->room->mutex), &g);
    // printf("Semaphore value ending %d\n", g);
    // sem_destroy(&(hThreadHunter->room->mutex));

    return NULL;
}   

void removeHunterFromRoom(HunterType *hunter){
    // int g;
    RoomType* oldRoom = hunter->room;
    // sem_getvalue(&(hunter->room->mutex), &g);
    // printf("INSIDE REMOVE BEFORE WAIT: %d\n", g);
    sem_wait(&(oldRoom->mutex));

    // sem_getvalue(&(hunter->room->mutex), &g);
    // printf("INSIDE REMOVE: %d\n", g);
    HunterListType *huntersInRoom = hunter->room->hunters;

    // printf("REMOVING HUNTER\nLIST BEFORE\n");
    // printf("Size of room hunter list %d\n", huntersInRoom->size);
    // for(int i = 0; i<huntersInRoom->size; i++){
    //     printf("   Name: %s\n", huntersInRoom->hunterList[i]->name);
    // }

    for(int i = 0; i<huntersInRoom->size; i++){
        if(huntersInRoom->hunterList[i] == hunter){
            for(int j = i+1; j<huntersInRoom->size; j++){
                huntersInRoom->hunterList[j-1] = huntersInRoom->hunterList[j];
            }

            huntersInRoom->size--;
            break;
        }
    }

    // printf("LIST AFTER\n");
    // for(int i = 0; i<huntersInRoom->size; i++){
    //     printf("   Name: %s\n", huntersInRoom->hunterList[i]->name);
    // }

    sem_post(&(oldRoom->mutex));
}


/* *******************************************************************************************
 * checkHunterWithGhost, if the hunter & ghost are in the same room return C_TRUE
 * HunterType *currentHunter (in) used to access the room which access's the ghost
 ********************************************************************************************/
int checkHunterWithGhost(HunterType *currentHunter) { 
    // If the hunter's room's ghost is NOT null, return C_TRUE
    if(currentHunter->room->ghost != NULL) {
        // printf("%s is in a room with the ghost\n", currentHunter->name);
        return C_TRUE;
    }
    // Otherwise return C_FALSE
    return C_FALSE;
}

/* *******************************************************************************************
 * containsThreeEvidence, if the currentHunter has a personalEvidenceList>=3 return C_TRUE
 * HunterType *currentHunter (in) used to determine the size of the personalEvidence list
 ********************************************************************************************/
int containsThreeEvidence(HunterType *currentHunter) {
    // Count the size of the list
    int evidenceCounter = 0;
    EvidenceNodeType *tempEvidenceNode = currentHunter->personalEvidence->head;

    // Iterate over list, until you reach the end, keep track of how many elements there are
    while(tempEvidenceNode != NULL) {
        // printf("contains 3 ev while \n");
        evidenceCounter++;
        tempEvidenceNode = tempEvidenceNode->next;
    }

    // If the evidence Counter is greater than or equal to 3, then return C_TRUE, otherwise return C_FALSE
    return (evidenceCounter >=3) ? C_TRUE : C_FALSE; 
}


/* *******************************************************************************************
 * communicateEvidence, if two hunters are in the same room, they may communicate evidence
 * HunterType *currentHunter (in/out) used to share evidence
 ********************************************************************************************/
int communicateEvidence(HunterType *currentHunter) {

    // // //PRINTING BULLSHIT
    printf("\n\nCommunicate Evidence\n");
    printf("Current Hunters Evidence To Start:\n");
    EvidenceNodeType *currEvHead = currentHunter->personalEvidence->head;
    while(currEvHead != NULL){
        printf("  %s %f %s\n", evidenceTypeToString(currEvHead->data->evidenceCategory), currEvHead->data->readingData,(isGhostly(currEvHead->data)) ? "IS GHOSTLY" : "");
        currEvHead = currEvHead->next;
    }

    // Get the current room of the hunter
    RoomType *currentRoom = currentHunter->room;
    
    // Select a random hunter from the list (0-3) inclusive
    int randomHunter = randInt(0, currentRoom->hunters->size);
    
    printf("Random Hunter pick while (For communicate) %d\n", currentHunter->room->hunters->size);
    // Re-run the random call until the random hunter does NOT equal the currentHunter
    while(currentHunter == currentRoom->hunters->hunterList[randomHunter]/* && currentRoom->hunters->size > 1*/){
        randomHunter = randInt(0, currentRoom->hunters->size);
        printf("Random Hunter pick while (For communicate) %d\n", currentHunter->room->hunters->size);
    }

    // Make a variable name hunterToCommunicate for the random hunterSelected
    HunterType *hunterToCommunicate = currentRoom->hunters->hunterList[randomHunter];

    //PRINTING BULLSHIT
    printf("To Communicate hunter evidence To start:\n");
    EvidenceNodeType *toCommunicateEv = currentRoom->hunters->hunterList[randomHunter]->personalEvidence->head;
    printf("Hunter %s is communicating with %s\n", currentHunter->name, currentRoom->hunters->hunterList[randomHunter]->name);
    while(toCommunicateEv != NULL){
        printf("  %s %f %s\n", evidenceTypeToString(toCommunicateEv->data->evidenceCategory), toCommunicateEv->data->readingData, (isGhostly(toCommunicateEv->data)) ? "IS GHOSTLY" : "");
        toCommunicateEv = toCommunicateEv->next;
    }

    // The traverse node for the hunterToCommunicate
    EvidenceNodeType *hunterTraverseNode = hunterToCommunicate->personalEvidence->head;
    
    // A copy of the evidenceList of the current hunter
    GhostEvidenceListType *evidenceCopy = makeACopyOfPersonalEvidence(currentHunter->personalEvidence);
    // The currentHunterTraverseNode (The head of the copied List)
    EvidenceNodeType *currentHunterTraverseNode = evidenceCopy->head;
    
    // Iterating through the hunterToCommunicate Evidence add each node evidence to the currentHunter personal evidence list if ghostly & is not already existing
    while(hunterTraverseNode != NULL) {
        // printf("Sharing hunterToCommunicate to currentHunter (For communicate)\n");
        // Only add the node to the current hunter if the data is unique & it is ghostly
        if(isGhostly(hunterTraverseNode->data) && !checkIfDuplicate(currentHunter->personalEvidence, hunterTraverseNode)) {

            // Allocate memory for a new node
            EvidenceNodeType *deepCopyEvNode = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
            
            // Set the data of the new node to the data of the traversing node
            deepCopyEvNode->data = hunterTraverseNode->data;
            deepCopyEvNode->next = NULL;

            // Add the deepCopyEvNode to the currentHunter's personalEvidence list
            addEvidenceToHunter(currentHunter->personalEvidence, deepCopyEvNode);
        }
        // Allow for looping
        hunterTraverseNode = hunterTraverseNode->next;
    }
    
 
    // Iterating through the currentHunter's COPIED LIST Evidence add each node evidence to the hunterToCommunicate personal evidence list if ghostly & is not already existing
    while(currentHunterTraverseNode != NULL){
        // printf("Sharing currentHunter to hunterToCommunicate (For communicate)\n");
        // Only add the node to the hunterToCommunicate if the data is unique & it is ghostly
        if(isGhostly(currentHunterTraverseNode->data) && !checkIfDuplicate(hunterToCommunicate->personalEvidence, currentHunterTraverseNode)){
            // Allocate memory for a new node
            EvidenceNodeType *deepCopyEvNode = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
            
            // Set the data of the new node to the data of the traversing node
            deepCopyEvNode->data = currentHunterTraverseNode->data;
            deepCopyEvNode->next = NULL;
             
            // Add the currentHunterTraverseNode to the hunterToCommunicate's personalEvidence list. No node creation is needed since we copied a brand new list already
            addEvidenceToHunter(hunterToCommunicate->personalEvidence, deepCopyEvNode);
        }
        // Allow for looping
        currentHunterTraverseNode = currentHunterTraverseNode->next;
    }

    // PRINTING BULLSHIT
    printf("Current Hunters Evidence To End:\n");
    currEvHead = currentHunter->personalEvidence->head;
    while(currEvHead != NULL){
        printf("  %s %f %s\n", evidenceTypeToString(currEvHead->data->evidenceCategory), currEvHead->data->readingData,(isGhostly(currEvHead->data)) ? "IS GHOSTLY" : "");
        currEvHead = currEvHead->next;
    }

    // PRINTING BULLSHIT
    printf("To Communicate hunter evidence To End:\n");
    toCommunicateEv = currentRoom->hunters->hunterList[randomHunter]->personalEvidence->head;
    while(toCommunicateEv != NULL){
        printf("  %s %f %s\n", evidenceTypeToString(toCommunicateEv->data->evidenceCategory), toCommunicateEv->data->readingData, (isGhostly(toCommunicateEv->data)) ? "IS GHOSTLY" : "");
        toCommunicateEv = toCommunicateEv->next;
    }

    //Return C_TRUE
    return C_TRUE;
}


/* *******************************************************************************************
 * moveHunter, Moves a hunter to an adjacent room
 * HunterType *currentHunter (in/out) used to move the hunter
 ********************************************************************************************/
int moveHunter(HunterType* currentHunter){
    // PRINTING BULLSHIT
    
    // Find the size of the room, so we can pick a random room
    RoomNodeType *traverseRoomNode = currentHunter->room->connectedRooms->head;
    int sizeCounter = 0;
    while(traverseRoomNode != NULL){
        // printf("Move Hunter while\n");
        sizeCounter++;
        traverseRoomNode = traverseRoomNode->next;
    }

    // Pick a random number between 0 and (sizeCounter-1) (inclusive)
    int randomRoomInt = randInt(0, sizeCounter);

    // Loop through the nodes until the for loop terminates, which leaves us at the randomRoomNode
    RoomNodeType *randomRoomNode = currentHunter->room->connectedRooms->head;
    for(int i = 0; i<randomRoomInt; i++){
        randomRoomNode = randomRoomNode->next;
    }

    RoomType *oldRoom = currentHunter->room;
    sem_wait(&(oldRoom->mutex));

    if(sem_trywait(&(randomRoomNode->data->mutex)) != 0){
        // printf("Waiting For Move Decision\n");
        sem_post(&(oldRoom->mutex));
        return C_FALSE;
    }

    // Remove the current hunter from his previous room
    int size = currentHunter->room->hunters->size;
    for(int i = 0; i<size; i++){
        if(currentHunter->room->hunters->hunterList[i] == currentHunter){
            for(int j = i+1; j<size; j++){
                // sem_wait(&(currentHunter->room->mutex));
                currentHunter->room->hunters->hunterList[j-1] = currentHunter->room->hunters->hunterList[j];
                // sem_post(&(currentHunter->room->mutex));
            }
            // Decrease the size
            currentHunter->room->hunters->size--;
            break;
        }
    }

    addHunterToRoom(randomRoomNode->data, currentHunter);

    // Decrease the boredom timer
    currentHunter->timer--;
  
    sem_post(&(oldRoom->mutex));
    sem_post(&(randomRoomNode->data->mutex));
    // printf("%s Moved from %s to %s\n", currentHunter->name, oldRoom->name, randomRoomNode->data->name);
    return C_TRUE;
}

/* *******************************************************************************************
 * collectEvidence, collects the evidence from a room
 * HunterType *currentHunter (in/out) used to grab evidence from a room
 ********************************************************************************************/
void collectEvidence(HunterType *currentHunter) {
    // printf("\n\nCOLLECTING EVIDENCE\n");
    // printf("STARTING: HUNTER EVIDENCE\n");
    // printGhostEvidenceList(currentHunter->personalEvidence, "   ");
    // If the room contains NO evidence give him random standard evidence of his tool type
    if(currentHunter->room->evidenceList->head == NULL) {

        // Allocate and create a new standard evidence
        EvidenceNodeType *newEvidence = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
        newEvidence->data = (EvidenceType*) malloc(sizeof(EvidenceType));
        newEvidence->data->evidenceCategory = currentHunter->evidence;
        newEvidence->data->readingData = generateStandardValue(newEvidence->data->evidenceCategory);
        newEvidence->next = NULL;
        // If the newly created evidence DOES NOT exist in the personalEvidence list add it to the personalEvidence list
        // if(!checkIfDuplicate(currentHunter->personalEvidence, newEvidence)){
        // printf("Evidence to add: ");
        // printEvidence(newEvidence->data);
        printf("ADDING %s %f\n", evidenceTypeToString(newEvidence->data->evidenceCategory), newEvidence->data->readingData);
        addEvidenceToHunter(currentHunter->personalEvidence, newEvidence);
        // printf("ENDING: HUNTER EVIDENCE\n");
        // printGhostEvidenceList(currentHunter->personalEvidence, "   ");
        // }else{
        //     printf("THE EVIDENCE WAS A DUPLICATE!\n");
        // }
        
        // Exit the function
        return;
    }

    // printf("ORIGINAL ROOM EVIDENCE\n");
    // printGhostEvidenceList(currentHunter->room->evidenceList, "   ");
    
    // If the list is not empty, iterate through until either nothing is found or an evidence of the same type of the current hunters tool is found
    EvidenceNodeType *tempEvidence = currentHunter->room->evidenceList->head;
    while(tempEvidence != NULL) {
        // If the currentHunters tool matches that of the type of the evidenceType AND the evidence does not already exist in the personalEvidence list
        if(tempEvidence->data->evidenceCategory == currentHunter->evidence) {
            // printf("%s: ADDING %s %f\n", currentHunter->name, evidenceTypeToString(tempEvidence->data->evidenceCategory), tempEvidence->data->readingData);
            // Remove the evidence from the room
            removeEvidenceFromRoom(currentHunter->room->evidenceList, tempEvidence);
            // Add the evidence found to the currentHunter's personalEvidence

            EvidenceNodeType *newEvidence = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
            newEvidence->data = (EvidenceType*) malloc(sizeof(EvidenceType));
            newEvidence->data->evidenceCategory = tempEvidence->data->evidenceCategory;
            newEvidence->data->readingData = generateStandardValue(tempEvidence->data->evidenceCategory);
            newEvidence->next = NULL;

            addEvidenceToHunter(currentHunter->personalEvidence, newEvidence);
            // printf("ROOM'S EVIDENCE LIST AFTER ADDITION\n");
            // printGhostEvidenceList(currentHunter->room->evidenceList, "   ");
            // printf("ENDING: HUNTER EVIDENCE\n");
            // printGhostEvidenceList(currentHunter->personalEvidence, "   ");

            // Reset the hunter boredom timer if evidence detected is ghostly
            if(isGhostly(tempEvidence->data)) {
                currentHunter->timer = BOREDOM_MAX;
            }
            return;
        }
        // Allow for looping
        tempEvidence = tempEvidence->next;
    }
}


/* *******************************************************************************************
 * generateStandardValue, generates a random standard float of a specific EvidenceClassType
 * EvidenceClassType evidenceClass (in), to determine the range which constitutes standard 
 ********************************************************************************************/
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
    // Return a C_MISC_ERROR if the class is somehow not found (Should be impossible)
    return C_MISC_ERROR;
}


/* *******************************************************************************************
 * removeEvidenceFromRoom, removes the given evidence from the given list
 * GhostEvidenceListType *list (in/out), used to locate and remove the evidence
 * EvidenceNodeType *evidence (in), used to compare the current node and itself
 ********************************************************************************************/
void removeEvidenceFromRoom(GhostEvidenceListType *list, EvidenceNodeType *evidence) {
    // Create a temporary node, such that we can iterate through
    EvidenceNodeType *tempEvidence = list->head;
    
    // If the evidence and the head are equal, disconnect the head (remove it)
    if(evidence == tempEvidence){
        list->head = list->head->next;
        return;
        //Free previous head
    }

    // Iterate through the list, if we find the evidence at the next position, set the next to next next (jump over it)
    while(tempEvidence->next != NULL) {
        // printf("remove evidence from room while\n");
        if(tempEvidence->next == evidence){
            //Prob need to free unless hunter uses it
            tempEvidence->next = tempEvidence->next->next;
            return;
        }
        // Allow for looping
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

/* *******************************************************************************************
 * addEvidenceToHunter, Adds the evidence to the hunter's personal list
 * GhostEvidenceListType *list (in/out), used to add the evidence
 * EvidenceNodeType *evidence (in), used add itself to the list
 ********************************************************************************************/
void addEvidenceToHunter(GhostEvidenceListType *list, EvidenceNodeType *evidence) {
    // For safety set the next to NULL
    evidence->next = NULL;

    // If the head is NULL set the Head & Tail to evidence (one element)
	if(list->head == NULL){
		list->head = evidence;
		list->tail = evidence;
    // Otherwise set the tail's next to evidence and change the tail pointer
	}else{
		list->tail->next = evidence;
		list->tail = evidence;
    }
}


/* *******************************************************************************************
 * isGhostly, determines if the given evidence is ghostly
 * EvidenceType *evidence (in), determines the type and the ghostly range based on the type
 ********************************************************************************************/
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

/* *******************************************************************************************
 * evidenceTypeToString, Converts the evidenceTypeEnum to a string
 * EvidenceClassType evidence, used to determine the right string to return
 ********************************************************************************************/
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

/* *******************************************************************************************
 * makeACopyOfPersonalEvidence, makes a copy of an evidenceList with a pointer to the list
 * GhostEvidenceListType *toCopyList (in), used to generate copies
 ********************************************************************************************/
GhostEvidenceListType* makeACopyOfPersonalEvidence(GhostEvidenceListType *toCopyList){

    // Allocate & Initalize a new list
    GhostEvidenceListType *copyListPtr = (GhostEvidenceListType*) malloc(sizeof(GhostEvidenceListType));
    initGhostList(copyListPtr);

    // Iterate through the toCopyList and copy each piece of data and add it to the previous list made
    EvidenceNodeType *traverseNode = toCopyList->head;
    while(traverseNode != NULL){
        // printf("copyPersonalEvidence while\n");
        EvidenceNodeType *tempEvNode = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
        tempEvNode->data = traverseNode->data;
        addEvidenceToHunter(copyListPtr, tempEvNode);
        traverseNode = traverseNode->next;
    }
    
    // Return the created list
    return copyListPtr;
}


/* *******************************************************************************************
 * checkIfDuplicate, returns C_TRUE if the data of the node is already found within the list
 * GhostEvidenceListType *list (in) used to iterate through all the nodes
 * EvidenceNodeType* node (in) used to compare at each iteration
 ********************************************************************************************/
int checkIfDuplicate(GhostEvidenceListType *list, EvidenceNodeType* node){
    // Iterate through the nodes
    EvidenceNodeType *traverseNode = list->head;
    while(traverseNode != NULL){
        // printf("checking duplicate while\n");
        // If at any point there exists a duplicate, return C_TRUE
        if(traverseNode->data->evidenceCategory == node->data->evidenceCategory && traverseNode->data->readingData == node->data->readingData){
            return C_TRUE;
        }
        // Allow for looping
        traverseNode = traverseNode->next;
    }

    //Otherwise return C_FALSE
    return C_FALSE;
}

/* *******************************************************************************************
 * printEvidence, used to print the given evidence
 * EvidenceType *evidence (in) used to print the evidence information
 ********************************************************************************************/
void printEvidence(EvidenceType *evidence){
    printf("  Type: %s\n", evidenceTypeToString(evidence->evidenceCategory));
    printf("  Reading Data: %f", evidence->readingData);
    printf("  %s\n\n", (isGhostly(evidence)) ? "IT IS GHOSTLY" : "");
}

/* *******************************************************************************************
 * printHunter, prints the hunters information
 * HunterType* hunter (in) used to get the infromation for the hunter
 ********************************************************************************************/
void printHunter(HunterType* hunter){
    printf("\nHunter Info\n");
    printf("Name: %s\n", hunter->name);
    printf("Fear Timer: %d\n", hunter->fear);
    printf("Boredom Timer: %d\n", hunter->timer);
    printf("Evidence Type: %s\n", evidenceTypeToString(hunter->evidence));
    printf("Evidence Collected:\n");
    // printGhostEvidenceList(hunter->personalEvidence);
    printf("Hunter's Room: %s\n", hunter->room->name);
    // printRoom(hunter->room);
    // printGhostEvidenceList(hunter->personalEvidence);
}

/* *******************************************************************************************
 * printHunterList, prints the hunter list
 * HunterListType *list (in) used to print each node's data of the entire list
 ********************************************************************************************/
void printHunterList(HunterListType *list) {
    printf("\nHunters\n");
    for(int i = 0; i < list->size; i++){
        printf("\n  Name: %s\n", (list->hunterList[i]->name));
        printf("   Room: %s\n", list->hunterList[i]->room->name);
        printf("   Tool Type: %s\n", evidenceTypeToString(list->hunterList[i]->evidence));
        printf("   Boredom Timer: %d\n", list->hunterList[i]->timer);
        printf("   Fear Timer: %d\n", list->hunterList[i]->fear);
        printf("   Personal Evidence:\n");
        printGhostEvidenceList(list->hunterList[i]->personalEvidence, "    ");
        printf("Contains 3 evidence: %s, fear>=100: %s, boredom<=0: %s\n", (containsThreeEvidence(list->hunterList[i])) ? "True" : "False", (list->hunterList[i]->fear>=100) ? "True" : "False", (list->hunterList[i]->timer<=0) ? "True" : "False");
    }
}