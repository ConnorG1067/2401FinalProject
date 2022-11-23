#include "defs.h"


void initRoom(RoomType *room, char *name) {
    //Name
    strcpy(room->name, name);


    //Lists
    room->connectedRooms = (RoomListType*) malloc(sizeof(RoomListType));
    initRoomList(&room->connectedRooms);
    room->evidenceList = (GhostEvidenceListType*) malloc(sizeof(GhostEvidenceListType));
    initGhostList(room->evidenceList);

    HunterListType* hunterList = (HunterListType*) malloc(sizeof(HunterListType));
    initHunterList(hunterList);
    room->hunters = hunterList;

    //Hunters and ghost
    room->ghost = (GhostType*) malloc(sizeof(GhostType));
}

void initRoomList(RoomListType **roomList){
    (*roomList)->head = NULL;
    (*roomList)->tail = NULL;
}

//Connected the two rooms together
//Uses append list to append each rooms connected rooms to both connectedRoomLists
void connectRooms(RoomNodeType *room1, RoomNodeType *room2){
    appendRoom(room1->data->connectedRooms, room2);
    appendRoom(room2->data->connectedRooms, room1);
}

// Append a room to the end of a room list
void appendRoom(RoomListType *roomList, RoomNodeType *room) {
    if(roomList->head == NULL) {
        roomList->head = room;
        roomList->tail = room;
    } else {
        roomList->tail->next = room;
        roomList->tail = room;
    }

    return;
}