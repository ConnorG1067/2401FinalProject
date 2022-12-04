#include "defs.h"

/* *******************************************************************************************
 *  initRoom initalizes the given room with a new name and its default options
 *  RoomType room (out), used to set parameters for the room
 *  char* name (in), the given name for the room
 ********************************************************************************************/
void initRoom(RoomType *room, char *name) {
    // Initalize the mutex
    sem_init(&(room->mutex), 0, 1);

    // Copy the name
    strcpy(room->name, name);

    // Room Lists
    room->connectedRooms = (RoomListType*) malloc(sizeof(RoomListType));
    initRoomList(&room->connectedRooms);

    room->evidenceList = (GhostEvidenceListType*) malloc(sizeof(GhostEvidenceListType));
    initGhostList(room->evidenceList);

    room->hunters = (HunterListType*) malloc(sizeof(HunterListType));
    initHunterList(room->hunters);

    // Room's ghost
    room->ghost = NULL;
}

/* *******************************************************************************************
 *  initRoomList initalizes the given room list by setting the head & tail to NULL
 *  RoomListType **roomList (out), used to set the head and tail of the list to NULL
 ********************************************************************************************/
void initRoomList(RoomListType **roomList){
    (*roomList)->head = NULL;
    (*roomList)->tail = NULL;
}

/* *******************************************************************************************
 * connectRooms conntects the two rooms given
 * RoomType *room1 (out) used to connect to room2
 * RoomType *room2 (out) used to connect to room1
 ********************************************************************************************/
void connectRooms(RoomType *room1, RoomType *room2){
    // Create two new nodes
    RoomNodeType *roomNode1 = (RoomNodeType*) malloc(sizeof(RoomNodeType));
    RoomNodeType *roomNode2 = (RoomNodeType*) malloc(sizeof(RoomNodeType));

    // Set the data
    roomNode1->data = room1;
    roomNode2->data = room2;

    // Set the next to NULL
    roomNode1->next = NULL;
    roomNode2->next = NULL;

    //Append both rooms to each other
    appendRoom(room1->connectedRooms, roomNode2);
    appendRoom(room2->connectedRooms, roomNode1);
}

/* *******************************************************************************************
 * appendRoom, appends a given node to a given list
 * RoomListType *roomList (out), the given list to append to
 * RoomNodeType *room (in), the given node to append to the list
 ********************************************************************************************/
void appendRoom(RoomListType *roomList, RoomNodeType *room) {

    // Setting the next to null
	room->next = NULL;
	
	// If the head is null the list is empty
	if(roomList->head == NULL){
		roomList->head = room;
		roomList->tail = room;
    // Otherwise add to the tail
	}else{
		roomList->tail->next = room;
		roomList->tail = room;
	}
    return;
}

/* *******************************************************************************************
 * addHunterToRoom, appends the given hunter to the given room
 * RoomType* room (out), the given room to append the hunter to
 * HunterType* hunter (in), the given hunter to add to the hunter list
 ********************************************************************************************/
int addHunterToRoom(RoomType* room, HunterType* hunter) {
    // If the size is not greater than MAX_HUNTERS add the hunter to the room and return C_TRUE
    if(room->hunters->size<MAX_HUNTERS){
        room->hunters->hunterList[room->hunters->size++] = hunter;
        hunter->room = room;
        return C_TRUE;
    // Otherwise return C_FALSE
    }else{
        return C_FALSE;
    }
}



/* *******************************************************************************************
 * printRoom, prints the rooms information to the terminal
 * RoomType *room (in), used to print the room information
 ********************************************************************************************/
void printRoom(RoomType *room) {
    printf("\nRoom\n");
    printf("Name: %s\n", room->name);
    printf("Connected Rooms:\n");
    printRoomList(room->connectedRooms);
    printf("Ghost Evidence List:\n");
    (room->hunters->size==0) ? printf("Hunters: Empty\n") : printHunterList(room->hunters);
    printf("Ghost: ");
    room->ghost == NULL ? printf("None\n") : printGhost(room->ghost);
}

/* *******************************************************************************************
 * printRoomList, prints a roomListType
 * RoomListType *roomList (in), used to print the room information
 ********************************************************************************************/
void printRoomList(RoomListType *roomList) {
    RoomNodeType *tempRoomNode = roomList->head;

    while(tempRoomNode != NULL){
        printf("\t%s\n", tempRoomNode->data->name);
        tempRoomNode = tempRoomNode->next;
    }
}

void freeConnectedRooms(RoomListType* list){
    RoomNodeType *tempNode;
	while(list->head != NULL){
		tempNode = list->head;
		list->head = list->head->next;
        free(tempNode);
	}
}

void freeRoomList(RoomListType *list) {
	RoomNodeType *tempNode;
	while(list->head != NULL){
		tempNode = list->head;
		list->head = list->head->next;
        sem_destroy(&(tempNode->data->mutex));
        freeEvidenceList(tempNode->data->evidenceList);
        freeConnectedRooms(tempNode->data->connectedRooms);
        free(tempNode->data->connectedRooms);
        free(tempNode->data->hunters);
        free(tempNode->data);
		free(tempNode);
        
	}
}
