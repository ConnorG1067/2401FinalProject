#include "defs.h"

void initGhost(GhostClassType *ghostType, RoomType *room GhostType *ghost) {
    ghost->ghostType = ghostType;
    ghost->room = room;
    ghost->boredomTimer = BOREDOM_MAX;
}