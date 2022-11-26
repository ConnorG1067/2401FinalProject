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
void connectRooms(RoomType *room1, RoomType *room2){
    RoomNodeType *roomNode1 = (RoomNodeType*) malloc(sizeof(RoomNodeType));
    RoomNodeType *roomNode2 = (RoomNodeType*) malloc(sizeof(RoomNodeType));

    roomNode1->data = room1;
    roomNode2->data = room2;

    roomNode1->next = NULL;
    roomNode2->next = NULL;

    
    appendRoom(room1->connectedRooms, roomNode2);
    appendRoom(room2->connectedRooms, roomNode1);
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

int addHunterToRoom(RoomType* room, HunterType* hunter){
    if(room->hunters->size<MAX_HUNTERS){
        room->hunters->hunterList[room->hunters->size++] = hunter;
        return C_TRUE;
    }else{
        return C_FALSE;
    }


}