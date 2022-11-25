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

#define C_TRUE              1
#define C_FALSE             0

#define C_MISC_ERROR       -1
#define C_NO_ROOM_ERROR    -2
#define C_ARR_ERROR        -3


// You may rename these types if you wish
typedef enum { EMF, TEMPERATURE, FINGERPRINTS, SOUND } EvidenceClassType;
typedef enum { POLTERGEIST, BANSHEE, BULLIES, PHANTOM } GhostClassType;

int randInt(int, int);          // Generates a pseudorandom integer between the parameters
float randFloat(float, float);  // Generates a pseudorandom float between the parameters


typedef struct EvidenceNode{
    struct EvidenceType* data;
    struct EvidenceNode* next;
} EvidenceNodeType;

//NodeTypes
typedef struct RoomNode{
    struct RoomType* data;
    struct RoomNode* next;
} RoomNodeType;

//LinkedListTypes
typedef struct GhostEvidenceList{
    EvidenceNodeType* head;
    EvidenceNodeType* tail;
} GhostEvidenceListType;

typedef struct RoomList{
    RoomNodeType* head;
    RoomNodeType* tail;
} RoomListType;

typedef struct HunterType {
  struct RoomType *room;
  EvidenceClassType evidence;
  GhostEvidenceListType *personalEvidence;
  char name[MAX_STR];
  int fear;   // Init to zero
  int timer;  // Init to BOREDOM_MAX
} HunterType;

typedef struct GhostType{
  GhostClassType *ghostType;
  struct RoomType *room;
  int boredomTimer; // initialize to BOREDOM_MAX
} GhostType;

//Entity types
typedef struct RoomType{
    char name[MAX_STR];
    RoomListType* connectedRooms;
    GhostEvidenceListType* evidenceList;
    struct HunterListType *hunters;
    struct GhostType *ghost;
} RoomType;

typedef struct HunterListType{
  int size;
  HunterType hunterList[MAX_HUNTERS];
} HunterListType;


typedef struct EvidenceType {
  EvidenceClassType evidenceCategory;
  int readingData;
} EvidenceType;


typedef struct {
    GhostType* ghost;
    HunterType* hunters[MAX_HUNTERS];
    int hunterListSize;
    RoomListType* rooms;
} BuildingType;


//PROVIDED FUNTIONS
void populateRooms(BuildingType*);

//INIT FUNCTIONS
void initHunterList(HunterListType*);
void initBuilding(BuildingType*);

void initRoom(RoomType*, char*);
void initRoomList(RoomListType**);
void connectRooms(RoomType*, RoomType*);
void appendRoom(RoomListType*, RoomNodeType*);


void initGhostList(GhostEvidenceListType *);
void initGhost(GhostClassType *, RoomType*, GhostType *);

//Threads
void *ghostThread(GhostType *);
void *hunterThread();

//Ghost Thread Helpers
void addRandomEvidence(GhostType*);
int generateValueOnType(EvidenceClassType);
void moveGhost(GhostType*, int);
int randomAdjacentRoom(GhostType*);
int checkGhostInRoom(GhostType*);

int checkHunterWithGhost(HunterType*);
int containsThreeEvidence(HunterType*);
