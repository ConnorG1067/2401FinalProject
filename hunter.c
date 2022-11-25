#include "defs.h"

void initHunter(RoomType *room, EvidenceClassType evidence,  GhostEvidenceListType* personalEvidence, char* name, int fear, int timer, HunterType *hunter) {
    strcpy(hunter->name, name);
    hunter->evidence = evidence;
    hunter->fear = 0;
    hunter->timer = BOREDOM_MAX;
}  

void initHunterList(HunterListType *hunters) {
    hunters->size = 0;
}

void *hunterThread(HunterType *currentHunter) {

    // If there are three different pieces of evidence
    //  OR fear level is greater than or equal to 100
    //  OR if boredom timer is <= 0
    // Exit the while loop, thus exiting the thread.
    while(!containsThreeEvidence() && !(currentHunter->fear >= 100) && !(currentHunter->timer <= 0)) {
        if(checkHunterWithGhost(currentHunter)) {
            currentHunter->fear++;
            currentHunter->timer = BOREDOM_MAX;
        } else {
            int hunterChoice = randInt(0,3)
            // Randomly (or using our own programmed logic) either 
            //  collect evidence
            //  move
            //  or communicate evidence if the hunter is in the same room as another hunter

        }
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