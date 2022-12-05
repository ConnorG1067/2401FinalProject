#include "defs.h"

/* *******************************************************************************************
 * initHunter, initalizes the given hunter
 * char* name (in) the name of the given HunterType
 * RoomType *room (in) the room hunter is meant to reside in
 * HunterType **hunter (out) the hunter that the function initalizes
 ********************************************************************************************/
void initHunter(char* name, RoomType *room, int uniqueRandomTool, int sleepTime, HunterType **hunter) {
    // Allocate memory
    HunterType *hunterPtr = (HunterType*) malloc(sizeof(HunterType));

    // Copy the name
    strcpy(hunterPtr->name, name);

    // Generate random evidence 
    hunterPtr->evidence = (EvidenceClassType) uniqueRandomTool;

    // Assign the room
    hunterPtr->room = room;

    
    // Allocate & Initalize a personalEvidence list
    GhostEvidenceListType *evidenceListPtr = (GhostEvidenceListType*) malloc(sizeof(GhostEvidenceListType));
    initPersonalEvidence(evidenceListPtr);
    hunterPtr->personalEvidence = evidenceListPtr;
    
    // Initialize fear & boredom timer
    hunterPtr->fear = 0;
    hunterPtr->timer = BOREDOM_MAX;
    hunterPtr->sleepTime = sleepTime;

    // Assign the hunter given to the hunterPtr made
    *hunter = hunterPtr; 
}  

/* *******************************************************************************************
 * initPersonalEvidence, initalizes a hunters personalEvidence list
 * GhostEvidenceListType* evidenceList (out), used to set the head and tail to NULL
 ********************************************************************************************/
void initPersonalEvidence(GhostEvidenceListType* evidenceList){
    // Initalize the head & tail to NULL
    evidenceList->head = NULL;
    evidenceList->tail = NULL;
}

/* *******************************************************************************************
 * initHunterList, initalizes a hunter list by setting the size to 0
 * HunterListType *hunters (in/out), used to set the size
 ********************************************************************************************/
void initHunterList(HunterListType *hunters) {
    //Initalize the size of the hunter list
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
    srand(time(NULL));
    // Set a local hunter pointer which points to the argument
    HunterType *hThreadHunter = (HunterType*) arg;

    // Exit if there are three different pieces of evidence
    //  OR fear level is greater than or equal to 100
    //  OR if boredom timer is <= 0
    // Exit the while loop, thus exiting the thread.
    while(!(containsThreeEvidence(hThreadHunter) || (hThreadHunter->fear >= 100) || (hThreadHunter->timer <= 0))) {
        sleep(hThreadHunter->sleepTime);
        // Check if the hThreadHunter and the ghost exist in the same room
        if(checkHunterWithGhost(hThreadHunter)) {
            // If they do increase the fear & reset the boredom timer
            hThreadHunter->fear++;
            hThreadHunter->timer = BOREDOM_MAX;
        }

        // Pick a random integer between 0-2 (inclusive)
        int hunterChoice = randInt(0,3); 
        // printf("HUNTERS CHOICE: %d\n", hunterChoice);

        switch(hunterChoice){
            case 0:
                // Collect Evidence
                sem_wait(&(hThreadHunter->room->mutex));
                collectEvidence(hThreadHunter);
                sem_post(&(hThreadHunter->room->mutex));
                break;
            
            case 1:
                // Move
                moveHunter(hThreadHunter);
                break;
            case 2:
                //Communicate
                sem_wait(&(hThreadHunter->room->mutex));
                // If there exists at least two hunters in the room, communicate evidence
                if(hThreadHunter->room->hunters->size > 1){
                    communicateEvidence(hThreadHunter);
                }else{
                    // Otherwise, notify the user there was an unsuccessful attempt
                    printf("%s attempted to communitcate in %s, but no one was found!\n", hThreadHunter->name, hThreadHunter->room->name);
                }
                sem_post(&(hThreadHunter->room->mutex));
                break;
        }

    }
    
    // Remove the hunter from the room to ensure the ghost thread ends
    removeHunterFromRoom(hThreadHunter, C_TRUE);
    return NULL;
}   


/* *******************************************************************************************
 * removeHunterFromRoom, removes a hunter from the given room he is in
 * HunterType *hunter, the hunter to remove from the room
 ********************************************************************************************/
void removeHunterFromRoom(HunterType *hunter, int lockBool){
    RoomType* oldRoom = hunter->room;
    // Take note of the old room
    if(lockBool){
        sem_wait(&(oldRoom->mutex));
    }
    
    
    // Lock the old room

    // Set huntersInRoom to the list of the current rooms hunters
    HunterListType *huntersInRoom = hunter->room->hunters;

    // Loop through the hunters in the given room until the hunter of interest is found
    for(int i = 0; i<huntersInRoom->size; i++){
        if(huntersInRoom->hunterList[i] == hunter){
            // Once found shift all elements back one, removing it from the array
            for(int j = i+1; j<huntersInRoom->size; j++){
                huntersInRoom->hunterList[j-1] = huntersInRoom->hunterList[j];
            }

            // Decrease the size and break
            huntersInRoom->size--;
            break;
        }
    }

    // Unlock the old room
    if(lockBool){
        sem_post(&(oldRoom->mutex));
    }
}


/* *******************************************************************************************
 * checkHunterWithGhost, if the hunter & ghost are in the same room return C_TRUE
 * HunterType *currentHunter (in) used to access the room which access's the ghost
 ********************************************************************************************/
int checkHunterWithGhost(HunterType *currentHunter) { 
    // If the hunter's room's ghost is NOT null, return C_TRUE
    if(currentHunter->room->ghost != NULL) {
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
    int ghostlyEvidenceCounter = 0;
    EvidenceNodeType *tempEvidenceNode = currentHunter->personalEvidence->head;

    // Iterate over list, until you reach the end, keep track of how many elements there are
    while(tempEvidenceNode != NULL) {
        if(isGhostly(tempEvidenceNode->data)){
            ghostlyEvidenceCounter++;
        }
        tempEvidenceNode = tempEvidenceNode->next;
    }

    // If the evidence Counter is greater than or equal to 3, then return C_TRUE, otherwise return C_FALSE
    return (ghostlyEvidenceCounter >=3) ? C_TRUE : C_FALSE; 
}


/* *******************************************************************************************
 * communicateEvidence, if two hunters are in the same room, they may communicate evidence
 * HunterType *currentHunter (in/out) used to share evidence
 ********************************************************************************************/
int communicateEvidence(HunterType *currentHunter) {
    // Get the current room of the hunter
    RoomType *currentRoom = currentHunter->room;
    
    // Select a random hunter from the list (0-3) inclusive
    int randomHunter = randInt(0, currentRoom->hunters->size);

    // Make a variable name hunterToCommunicate for the random hunterSelected
    HunterType *hunterToCommunicate = currentRoom->hunters->hunterList[randomHunter];

    // The traverse node for the hunterToCommunicate
    EvidenceNodeType *hunterTraverseNode = hunterToCommunicate->personalEvidence->head;
    
    // A copy of the evidenceList of the current hunter
    GhostEvidenceListType *evidenceCopy = makeACopyOfPersonalEvidence(currentHunter->personalEvidence);
    // The currentHunterTraverseNode (The head of the copied List)
    EvidenceNodeType *currentHunterTraverseNode = evidenceCopy->head;
    
    // Iterating through the hunterToCommunicate Evidence add each node evidence to the currentHunter personal evidence list if ghostly & is not already existing
    while(hunterTraverseNode != NULL) {
        // Only add the node to the current hunter if the data is unique & it is ghostly
        if(isGhostly(hunterTraverseNode->data) && !checkIfDuplicate(currentHunter->personalEvidence, hunterTraverseNode)) {
            // Allocate memory for a new node
            EvidenceNodeType *deepCopyEvNode = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
            

            // Told in extra clarifications we could duplicate data 
            EvidenceType *deepCopyEvidence = (EvidenceType*) malloc(sizeof(EvidenceType));
            deepCopyEvidence->evidenceCategory = hunterTraverseNode->data->evidenceCategory;
            deepCopyEvidence->readingData = hunterTraverseNode->data->readingData;
            
            // Set the data of the new node to the data of the traversing node
            deepCopyEvNode->data = deepCopyEvidence;
            deepCopyEvNode->next = NULL;

            // Add the deepCopyEvNode to the currentHunter's personalEvidence list
            addEvidenceToHunter(currentHunter->personalEvidence, deepCopyEvNode);
            printf("%s communicated %s %f to %s\n", hunterToCommunicate->name, evidenceTypeToString(hunterTraverseNode->data->evidenceCategory), hunterTraverseNode->data->readingData, currentHunter->name);
        }
        // Allow for looping
        hunterTraverseNode = hunterTraverseNode->next;
    }
    
 
    // Iterating through the currentHunter's COPIED LIST Evidence add each node evidence to the hunterToCommunicate personal evidence list if ghostly & is not already existing
    while(currentHunterTraverseNode != NULL){
        // Only add the node to the hunterToCommunicate if the data is unique & it is ghostly
        if(isGhostly(currentHunterTraverseNode->data) && !checkIfDuplicate(hunterToCommunicate->personalEvidence, currentHunterTraverseNode)){
            // Allocate memory for a new node
            EvidenceNodeType *deepCopyEvNode = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));

            // Told in extra clarifications we could duplicate data 
            EvidenceType *deepCopyEvidence = (EvidenceType*) malloc(sizeof(EvidenceType));
            deepCopyEvidence->evidenceCategory = currentHunterTraverseNode->data->evidenceCategory;
            deepCopyEvidence->readingData = currentHunterTraverseNode->data->readingData;
            
            // Set the data of the new node to the data of the traversing node
            deepCopyEvNode->data = deepCopyEvidence;

            // Add the currentHunterTraverseNode to the hunterToCommunicate's personalEvidence list. No node creation is needed since we copied a brand new list already
            addEvidenceToHunter(hunterToCommunicate->personalEvidence, deepCopyEvNode);
            printf("%s communicated %s %f to %s\n", currentHunter->name, evidenceTypeToString(currentHunterTraverseNode->data->evidenceCategory), currentHunterTraverseNode->data->readingData, hunterToCommunicate->name);
        }
        // Allow for looping
        currentHunterTraverseNode = currentHunterTraverseNode->next;
    }

    // Free the copied evidence list
    freeEvidenceList(evidenceCopy);

    //Return C_TRUE
    return C_TRUE;
}


/* *******************************************************************************************
 * moveHunter, Moves a hunter to an adjacent room
 * HunterType *currentHunter (in/out) used to move the hunter
 ********************************************************************************************/
int moveHunter(HunterType* currentHunter){
    
    // Find the size of the room, so we can pick a random room
    RoomNodeType *traverseRoomNode = currentHunter->room->connectedRooms->head;
    int sizeCounter = 0;
    while(traverseRoomNode != NULL){
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

    // Take note of the oldRoom
    RoomType *oldRoom = currentHunter->room;

    // Lock the oldRoom
    sem_wait(&(oldRoom->mutex));

    // If the new room is not available unlock the old room and return C_FALSE
    if(sem_trywait(&(randomRoomNode->data->mutex)) != 0){
        // printf("trywait\n");
        sem_post(&(oldRoom->mutex));
        return C_FALSE;
    }

    // Remove the current hunter from his previous room
    removeHunterFromRoom(currentHunter, C_FALSE);
    // Add the hunter to the new random room
    addHunterToRoom(randomRoomNode->data, currentHunter);

    // Display the movement to the user
    printf("%s moved from %s to %s\n", currentHunter->name, oldRoom->name, currentHunter->room->name);

    // Decrease the boredom timer
    currentHunter->timer--;
  
    // Unlock both rooms
    sem_post(&(oldRoom->mutex));
    sem_post(&(randomRoomNode->data->mutex));

    return C_TRUE;
}

/* *******************************************************************************************
 * collectEvidence, collects the evidence from a room
 * HunterType *currentHunter (in/out) used to grab evidence from a room
 ********************************************************************************************/
int collectEvidence(HunterType *currentHunter) {
    // If the room contains NO evidence give him random standard evidence of his tool type
    if(currentHunter->room->evidenceList->head == NULL) {
        // Allocate and create a new standard evidence
        EvidenceNodeType *newEvidence = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
        newEvidence->data = (EvidenceType*) malloc(sizeof(EvidenceType));
        newEvidence->data->evidenceCategory = currentHunter->evidence;
        newEvidence->data->readingData = generateStandardValue(newEvidence->data->evidenceCategory);
        newEvidence->next = NULL;

        // Add the newly randomly generated standard evidence to the list
        addEvidenceToHunter(currentHunter->personalEvidence, newEvidence);
        printf("%s found standard %s %f in %s\n", currentHunter->name, evidenceTypeToString(newEvidence->data->evidenceCategory), newEvidence->data->readingData, currentHunter->room->name);

        // Exit the function
        return C_TRUE;
    }

    // If the list is not empty, iterate through until either nothing is found or an evidence of the same type of the current hunters tool is found
    EvidenceNodeType *tempEvidence = currentHunter->room->evidenceList->head;

    while(tempEvidence != NULL) {
        // If the currentHunters tool matches that of the type of the evidenceType AND the evidence does not already exist in the personalEvidence list
        if(tempEvidence->data->evidenceCategory == currentHunter->evidence) {
             // Alloc
            EvidenceNodeType *newEvidenceNode = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
            EvidenceType *newEvidence = (EvidenceType*) malloc(sizeof(EvidenceType));

            // Set new evidence
            newEvidence->readingData = tempEvidence->data->readingData;
            newEvidence->evidenceCategory = tempEvidence->data->evidenceCategory;

            // Set the nodes data
            newEvidenceNode->data = newEvidence;

            // Reset the hunter boredom timer if evidence detected is ghostly
            if(isGhostly(tempEvidence->data)) {
                currentHunter->timer = BOREDOM_MAX;
            }

            //Add the evidence to the hunters list and remove it from the rooms list
            addEvidenceToHunter(currentHunter->personalEvidence, newEvidenceNode);
            printf("%s found %s %f in %s (Droped by the ghost)\n", currentHunter->name, evidenceTypeToString(newEvidenceNode->data->evidenceCategory), newEvidenceNode->data->readingData, currentHunter->room->name);
            removeEvidenceFromRoom(currentHunter->room->evidenceList, tempEvidence);
            return C_TRUE;
        }
        // Allow for looping
        tempEvidence = tempEvidence->next;
    }

    return C_FALSE;
}


/* *******************************************************************************************
 * generateStandardValue, generates a random standard float of a specific EvidenceClassType
 * EvidenceClassType evidenceClass (in), to determine the range which constitutes standard 
 ********************************************************************************************/
float generateStandardValue(EvidenceClassType evidenceClass){
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
int removeEvidenceFromRoom(GhostEvidenceListType *list, EvidenceNodeType *evidence) {
    // Create a temporary node, such that we can iterate through
    EvidenceNodeType *tempEvidence = list->head;
    
    // If the evidence and the head are equal, disconnect the head (remove it)
    if(evidence == list->head){
        free(list->head->data);
        EvidenceNodeType *freeNode = list->head;
        list->head = list->head->next;
        free(freeNode);
        return C_TRUE;
    }

    // Iterate through the list, if we find the evidence at the next position, set the next to next next (jump over it)
    while(tempEvidence->next != NULL) {
        if(tempEvidence->next == evidence){
            // If the next item is the evidence to remove & that evidence is the tail disconnect the tail
            if(tempEvidence->next == list->tail){
                free(list->tail->data);
                EvidenceNodeType *freeNode = list->tail;
                list->tail = tempEvidence;
                tempEvidence->next = NULL;
                free(freeNode);
                return C_TRUE;
            }
            // Otherwise, it must be in the body of the list
            EvidenceNodeType *freeNode = tempEvidence->next;
            free(freeNode->data);
            tempEvidence->next = tempEvidence->next->next;
            free(freeNode);
            return C_TRUE;
        }
        // Allow for looping
        tempEvidence = tempEvidence->next;
    }

    // No removal was made
    return C_FALSE;
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
    // Check the evidence category and then check if the reading data is ghostly (no intersection with standard)
    switch(evidence->evidenceCategory) {
        case 0:
            if(evidence->readingData > 4.90 && evidence->readingData <= 5.00){
                return C_TRUE;
            }
            break;
        case 1:
            if(evidence->readingData >=-10 && evidence->readingData<0){
                return C_TRUE;
            }
            break;
        case 2:
            if(evidence->readingData == 1.00) {
                return C_TRUE;
            }
            break;
        case 3:
            if(evidence->readingData > 70.00 && evidence->readingData <= 75.00) {
                return C_TRUE;
            }
            break;
    }

    // Return false if the data was not ghostly
    return C_FALSE;
}

/* *******************************************************************************************
 * evidenceTypeToString, Converts the evidenceTypeEnum to a string
 * EvidenceClassType evidence, used to determine the right string to return
 ********************************************************************************************/
char* evidenceTypeToString(EvidenceClassType evidence){
    // Return the string version of the enum
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
        default:
            return "INVALID";
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
        // Alloc
        EvidenceNodeType *tempEvNode = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
        EvidenceType *tempEvidence = (EvidenceType*) malloc(sizeof(EvidenceType));

        // Set new evidence
        tempEvidence->readingData = traverseNode->data->readingData;
        tempEvidence->evidenceCategory = traverseNode->data->evidenceCategory;

        // Set the nodes data
        tempEvNode->data = tempEvidence;

        // Add node to the list
        addEvidenceToHunter(copyListPtr, tempEvNode);

        // Loop
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
    printf("  Type: %s %f", evidenceTypeToString(evidence->evidenceCategory), evidence->readingData);
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
    printGhostEvidenceList(hunter->personalEvidence, "   ");
    printf("Hunter's Room: %s\n", hunter->room->name);
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

/* *******************************************************************************************
 * freeHunter, deallocates data associated with hunter
 * HunterType *hunter (in) deallocated the data in hunter
 ********************************************************************************************/
void freeHunter(HunterType *hunter) {
    freeEvidenceList(hunter->personalEvidence);
    free(hunter);
}

/* *******************************************************************************************
 * freeEvidenceData, deallocates data associated with ghost evidence list
 * GhostEvidenceListType *list (in) contains all the data to be deallocated
 ********************************************************************************************/
void freeEvidenceData(GhostEvidenceListType *list){
    EvidenceNodeType *tempNode = list->head;
	while(tempNode != NULL){
		free(tempNode->data);
		tempNode = tempNode->next;
	}
}

/* *******************************************************************************************
 * freeEvidenceNodes, deallocates nodes associated with ghost evidence list
 * GhostEvidenceListType *list (in) contains all the nodes to be deallocated
 ********************************************************************************************/
void freeEvidenceNodes(GhostEvidenceListType *list){
	EvidenceNodeType *tempNode;
	while(list->head != NULL){
		tempNode = list->head;
		list->head = list->head->next;
		free(tempNode);
	}
}

/* *******************************************************************************************
 * freeEvidenceList, deallocates nodes and data associated with ghost evidence list
 * GhostEvidenceListType *list (in) is the list to be deallocated
 ********************************************************************************************/
void freeEvidenceList(GhostEvidenceListType *list){
    freeEvidenceData(list);
    freeEvidenceNodes(list);
    free(list);
}

