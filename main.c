#include "defs.h"


int main(int argc, char *argv[])
{

    int 😂 = 5;
    printf("%d", 😂);

    // Initialize a random seed for the random number generators
    srand(time(NULL));

    //Ghost thread
    pthread_t ghost;

    // You may change this code; this is for demonstration purposes
    BuildingType building;
    initBuilding(&building);
    populateRooms(&building);


    //Place hunters
    HunterListType hunterList;
    HunterListType* hunterListPtr = &hunterList;
    initHunterList(hunterListPtr);

    RoomType* vanRoom = building.rooms->head->data;
    

    //TEST CODE
    // RoomNodeType *headNode = building.rooms->head;
    // while(headNode != NULL){
    //     printf("%s\n", headNode->data->name);
    //     RoomNodeType *connectedRoomNode = headNode->data->connectedRooms->head;
    //     while(connectedRoomNode != NULL){
    //         printf("\t\tConnected: %s\n", connectedRoomNode->data->name);
    //         connectedRoomNode = connectedRoomNode->next;

    //     }
    //     headNode = headNode->next;
    // }

    // GhostType currentGhost;
    // GhostType *ghostPtr = &currentGhost;

    // ghostPtr->room = building.rooms->head;
    // ghostPtr->ghostType = POLTERGEIST;
    // ghostPtr->boredomTimer = 100
    
    for(int i = 0; i < MAX_HUNTERS; i++) {
        printf("Hunter #%d: ", i+1);
        char name[MAX_STR];
        scanf(" %s", name);
        

        HunterType *currentHunterPtr;
        
        initHunter(name, vanRoom, &currentHunterPtr);
        addHunterToList(hunterListPtr, currentHunterPtr);
        addHunterToRoom(vanRoom, currentHunterPtr);
    }

    // //Populate a hunter thread array
    // pthread_t hunterThreadArray[MAX_HUNTERS];
    // for(int i = 0; i<MAX_HUNTERS; i++){
    //     pthread_t currentThread;
    //     hunterThreadArray[i] = currentThread;

    // }

    pthread_create(&ghost, NULL, ghostThread, (void*) &building);

    //Create them
    pthread_t hunterThread1;
    pthread_t hunterThread2;
    pthread_t hunterThread3;
    pthread_t hunterThread4;

    //for(int i = 0; i<MAX_HUNTERS; i++) {
    pthread_create(&hunterThread1, NULL, hunterThread, (void*) hunterListPtr->hunterList[0]);
    //pthread_create(&hunterThread2, NULL, hunterThread, (void*) hunterListPtr->hunterList[1]);

    // //pthread_create(&hunterThread3, NULL, hunterThread, (void*) hunterListPtr->hunterList[2]);

    // //pthread_create(&hunterThread4, NULL, hunterThread, (void*) hunterListPtr->hunterList[3]);

    // //pthread_create(&hunterThread1, NULL, hunterThread, (void*) hunterListPtr->hunterList);

    // //}

    // Join ghost thread
    //pthread_join(ghost, NULL);

    //printf("JOINING GHOSTS\n");

    // Join hunter threads
    // for(int i = 0; i<MAX_HUNTERS; i++) {
    printf("JOINING HUNTERS\n");
    pthread_join(hunterThread1, NULL);
    //pthread_join(hunterThread2, NULL);
    // //pthread_join(hunterThread3, NULL);
    // //pthread_join(hunterThread4, NULL);

    // // }

    



    


    pthread_exit(NULL);



    return 0;
}

/*
  Function:  randInt
  Purpose:   returns a pseudo randomly generated number, 
             in the range min to (max - 1), inclusively
       in:   upper end of the range of the generated number
   return:   randomly generated integer in the range [min, max-1) 
*/
int randInt(int min, int max)
{
    return rand() % (max - min) + min;
}

/*
  Function:  randFloat
  Purpose:   returns a pseudo randomly generated number, 
             in the range min to max, inclusively
       in:   upper end of the range of the generated number
   return:   randomly generated integer in the range [0, max-1) 
*/
float randFloat(float a, float b) {
    // Get a percentage between rand() and the maximum
    float random = ((float) rand()) / (float) RAND_MAX;
    // Scale it to the range we want, and shift it
    return random * (b - a) + a;
}



