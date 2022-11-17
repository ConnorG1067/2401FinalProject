#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_STR            64
#define FEAR_RATE           1
#define MAX_FEAR          100
#define MAX_HUNTERS         4
#define USLEEP_TIME     50000
#define BOREDOM_MAX        99

// You may rename these types if you wish
typedef enum { EMF, TEMPERATURE, FINGERPRINTS, SOUND } EvidenceClassType;
typedef enum { POLTERGEIST, BANSHEE, BULLIES, PHANTOM } GhostClassType;

int randInt(int, int);          // Generates a pseudorandom integer between the parameters
float randFloat(float, float);  // Generates a pseudorandom float between the parameters

void populateRooms(BuildingType*);   // Populates the building with sample data for rooms

//Entity types

typedef struct {
    GhostType* ghost;
    HunterType* hunters[MAX_HUNTERS];
    RoomListType* rooms;
} BuildingType;

typedef struct {
    char name[MAX_STR];
    HunterType hunters[MAX_HUNTERS];
    GhostEvidenceList* evidenceList;
    RoomList* connectedRooms;
    GhostType* ghost;
} RoomType;



//NodeTypes
typedef struct RoomNode{
    RoomType* data;
    struct RoomNode* next;
} RoomNodeType;

typedef struct EvidenceNode{
    EvidenceType* data;
    struct EvidenceNode* next;`
} EvidenceNodeType



//LinkedListTypes
typedef struct GhostEvidenceList{
  EvidenceNode* head;
  EvidenceNode* tail;
} GhostEvidenceListType;


typedef struct RoomList{
  RoomNode* head;
  RoomNode* tail;
} RoomListType;




