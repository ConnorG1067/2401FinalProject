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

void *hunterThread() {

}