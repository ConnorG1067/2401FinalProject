#include "defs.h"


int main(int argc, char *argv[])
{

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
    

    // // TEST CODE
    // RoomNodeType *headNode = building.rooms->head;
    // while(headNode != NULL){
    //     printf("Room: %s\n", headNode->data->name);
    //     RoomNodeType *connectedRoomNode = headNode->data->connectedRooms->head;
    //     while(connectedRoomNode != NULL){
    //         printf("\tConnected: %s\n", connectedRoomNode->data->name);
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
        char name[MAX_STR];
        printf("%d. Hunter: ", i+1);
        scanf("%s", name);
        

        HunterType *currentHunterPtr;
        
        initHunter(name, vanRoom, &currentHunterPtr);
        //BREAKS!
        addHunterToList(hunterListPtr, currentHunterPtr);
        addHunterToRoom(vanRoom, currentHunterPtr);
    }


    //Populate a hunter thread array
    pthread_t hunterThreadArray[MAX_HUNTERS];
    for(int i = 0; i<MAX_HUNTERS; i++){
        pthread_t currentThread;
        hunterThreadArray[i] = currentThread;

    }

    pthread_create(&ghost, NULL, ghostThread, (void*) &building);


    for(int i = 0; i<MAX_HUNTERS; i++) {
        pthread_create(hunterThreadArray + i, NULL, hunterThread, (void*) hunterListPtr->hunterList[i]);
    }


    for(int i = 0; i<MAX_HUNTERS; i++) {
        pthread_join(hunterThreadArray[i], NULL);
    }

    pthread_join(ghost, NULL);
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



