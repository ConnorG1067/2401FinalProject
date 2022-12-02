
#include "defs.h"


// #define CALCULATIONS 100000000
// #define BATCH_SIZE   100000
// #define MAX_CORES    12

// long long calculations_left = CALCULATIONS;
// sem_t mutex;
// int total_batches = 0;

// void *process_batch(void *arg) {
//     long long i, start_point;

//     for (;;) {
//         // acquire lock
//         sem_wait(&mutex);

//         // check if all calculations have been accomplished, and return if so
//         if (calculations_left <= 0) {
//             sem_post(&mutex);
//             return NULL;
//         }

//         // retrieve a batch
//         start_point = calculations_left - BATCH_SIZE;
//         calculations_left -= BATCH_SIZE;
//         ++total_batches;

//         printf("processing %lld to %lld\n", start_point, start_point + BATCH_SIZE);

//         // release lock
//         sem_post(&mutex);

//         // process batch
//         for (i = start_point; i < start_point + BATCH_SIZE; ++i) {
//             sqrt(i);
//         }
//     }
// }

// int main(void) {
//     int i = 0;
//     sem_init(&mutex, 0, 1);
//     // create a thread group the size of MAX_CORES
//     pthread_t *thread_group = malloc(sizeof(pthread_t) * MAX_CORES);

//     // start all threads to begin work
//     for (i = 0; i < MAX_CORES; ++i) {
//         pthread_create(&thread_group[i], NULL, process_batch, NULL);
//     }

//     // wait for all threads to finish
//     for (i = 0; i < MAX_CORES; ++i) {
//         pthread_join(thread_group[i], NULL);
//     }

//     printf("total batches: %d\n", total_batches);

//     return EXIT_SUCCESS;
// }


int main(int argc, char *argv[])
{
    // Initialize a random seed for the random number generators
    for(int p = 0; p<1000; p++){
        srand(time(NULL));

        //Ghost thread
        pthread_t pThreadghost;

        // You may change this code; this is for demonstration purposes
        BuildingType building;
        initBuilding(&building);
        populateRooms(&building);


        //Place hunters
        HunterListType hunterList;
        HunterListType* hunterListPtr = &hunterList;
        initHunterList(hunterListPtr);

        // Ghost 
        GhostType ghost;
        GhostType *ghostPtr = &ghost;
        initGhost(randInt(0,4), getRandomRoom(building.rooms->head)->data, ghostPtr);

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
        // printf("ITERATION %d\n", p);
        for(int i = 0; i < MAX_HUNTERS; i++) {
            char name[MAX_STR];
            printf("%d. Hunter: \n", i+1);
            // scanf("%s", name);
            sprintf(name, "%d\n", i+1);

            

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

        // pthread_create(&pThreadghost, NULL, ghostThread, (void*) ghostPtr);

        for(int i = 0; i<MAX_HUNTERS; i++) {
            pthread_create(hunterThreadArray + i, NULL, hunterThread, (void*) hunterListPtr->hunterList[i]);
            // pthread_join(hunterThreadArray[i], NULL);
        }


        for(int i = 0; i<MAX_HUNTERS; i++) {
            pthread_join(hunterThreadArray[i], NULL);
        }

        // pthread_join(pThreadghost, NULL);
        printHunterList(hunterListPtr);
        
        RoomNodeType *traverseRoom = building.rooms->head;
        // while(traverseRoom != NULL){
        //     printf("Hunter in %s size: %d\n", traverseRoom->data->name, traverseRoom->data->hunters->size);
        //     traverseRoom = traverseRoom->next;
        // }

        // printf("hunters fear:\n");
        // for(int i = 0; i < 4; i++){
        //     printf("%s: d")
        // }

        // for(int i = 0; i<hunterListPtr->size; i++){
        //     // printf("%s\n", hunterListPtr->hunterList[i]->room->name);
        //     if(strcmp(hunterListPtr->hunterList[i]->room->name , "Utility Room") == 0){
        //         printf("%s\n", hunterListPtr->hunterList[i]->room->name);
        //     }
        //     // printHunter(hunterListPtr->hunterList[i]);   
        // }


        
    //    int printedHunters = 0;
    //     for(int i = 0; i<hunterListPtr->size; i++){
    //         if(hunterListPtr->hunterList[i]->fear >= 100){
    //             printHunter(hunterListPtr->hunterList[i]);
    //             printedHunters++;
    //         }
    //     }
    //     if(printedHunters == 4){
    //          determineGhost()
    //     } 
        // printf("\n\nROOM SEMAPHORE VALUES\n");
        // RoomNodeType *roomHead = building.rooms->head;
        // while(roomHead != NULL){
        //     int p;
        //     sem_getvalue(&(roomHead->data->mutex), &p);
        //     printf("Room %s with sem_value = %d\n", roomHead->data->name, p);
        //     sem_destroy(&(roomHead->data->mutex));
        //     roomHead = roomHead->next;
        // }
    }
    
    


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



