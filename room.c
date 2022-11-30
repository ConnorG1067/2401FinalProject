#include "defs.h"


void initRoom(RoomType *room, char *name) {
    sem_init(&room->mutex, 0, 1);
    // Malloc some data probably

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
    room->ghost = NULL;
}

void initRoomList(RoomListType **roomList){
    (*roomList)->head = NULL;
    (*roomList)->tail = NULL;
    // printf("TESTING: %s\n", (*roomList)->head->data->name);
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

    //Setting the values of new node
	room->next = NULL;
	
	//Setting the end of the list to the new node
	if(roomList->head == NULL){
		roomList->head = room;
		roomList->tail = room;
	}else{
		roomList->tail->next = room;
		roomList->tail = room;

	}

    return;
}

int addHunterToRoom(RoomType* room, HunterType* hunter) {
    if(room->hunters->size<MAX_HUNTERS){
        room->hunters->hunterList[room->hunters->size++] = hunter;
        hunter->room = room;
        return C_TRUE;
    }else{
        return C_FALSE;
    }
}

void printRoom(RoomType *room) {
    printf("\nRoom\n");
    printf("Name: %s\n", room->name);
    printf("Connected Rooms:\n");
    printRoomList(room->connectedRooms);
    printf("Ghost Evidence List:\n");
    printGhostEvidenceList(room->evidenceList);
    (room->hunters->size==0) ? printf("Hunters: Empty\n") : printHunterList(room->hunters);
    printf("Ghost: ");
    room->ghost == NULL ? printf("None\n") : printGhost(room->ghost);
}


void printRoomList(RoomListType *roomList) {
    RoomNodeType *tempRoomNode = roomList->head;

    

    while(tempRoomNode != NULL){
        printf("\t%s\n", tempRoomNode->data->name);
        tempRoomNode = tempRoomNode->next;
    }
}