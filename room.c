#include "defs.h"

void initRoom(RoomType *room, char *name) {
    //Name
    room->name = name;

    //Lists
    room->connectedRooms = (RoomListType*) malloc(sizeof(RoomListType));
    initRoomList(room->connectedRooms);
    room->evidenceList = (GhostEvidenceListType*) malloc(sizeof(GhostEvidenceListType));
    initGhostList(&room->evidenceList);

    //Hunters and ghost
    room->hunters = (HunterType*) malloc(sizeof(HunterType));
    room->ghost = (GhostType*) malloc(sizeof(GhostType));
}

void initRoomList(RoomListType *roomList){
    roomList->head = NULL;
    roomList->tail = NULL;
}