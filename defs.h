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
#define TOTAL_ROOMS        13

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

typedef struct RoomNode{
    struct RoomType* data;
    struct RoomNode* next;
} RoomNodeType;

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
  GhostClassType ghostType;
  struct RoomType *room;
  int boredomTimer; // initialize to BOREDOM_MAX
} GhostType;

typedef struct RoomType{
    sem_t mutex;
    char name[MAX_STR];
    RoomListType* connectedRooms;
    GhostEvidenceListType* evidenceList;
    struct HunterListType *hunters;
    struct GhostType *ghost;
} RoomType;

typedef struct HunterListType{
  int size;
  HunterType *hunterList[MAX_HUNTERS];
} HunterListType;


typedef struct EvidenceType {
  EvidenceClassType evidenceCategory;
  float readingData;
} EvidenceType;

typedef struct {
    GhostType* ghost;
    HunterType* hunters[MAX_HUNTERS];
    int hunterListSize;
    RoomListType* rooms;
} BuildingType;

// PROVIDED FUNTIONS
void populateRooms(BuildingType*);

// INIT FUNCTIONS
void initHunterList(HunterListType*);
void initBuilding(BuildingType*);
void initRoom(RoomType*, char*);
void initRoomList(RoomListType**);
void initGhostList(GhostEvidenceListType *);
void initGhost(GhostClassType, RoomType*, GhostType *);
void initPersonalEvidence(GhostEvidenceListType* );
void initHunter(char* , RoomType *, HunterType **);


// THREADS
void *ghostThread();
void *hunterThread();

// MISC FUNCTIONS
void connectRooms(RoomType*, RoomType*);
void appendRoom(RoomListType*, RoomNodeType*);
int addHunterToList(HunterListType*, HunterType*);
int addHunterToRoom(RoomType*, HunterType*);
RoomNodeType* getRandomRoom(RoomNodeType*);
GhostClassType getRandomGhostType();

// GHOST THREAD HELPERS
void addRandomEvidence(GhostType*);
int generateValueOnType(EvidenceClassType);
void moveGhost(GhostType*);
int checkGhostInRoom(GhostType*);

// HUNTER THREAD HELPERS
int checkHunterWithGhost(HunterType*);
int containsThreeEvidence(HunterType*);
int communicateEvidence(HunterType*);
void collectEvidence(HunterType*);
int moveHunter(HunterType*);
void addEvidenceToHunter(GhostEvidenceListType*, EvidenceNodeType*);
void removeEvidenceFromRoom(GhostEvidenceListType *, EvidenceNodeType *);
int isGhostly(EvidenceType*);
int generateStandardValue(EvidenceClassType);
char* evidenceTypeToString(EvidenceClassType); 
char* ghostTypeToString(GhostClassType ghost);
GhostEvidenceListType* makeACopyOfPersonalEvidence(GhostEvidenceListType *);
int checkIfDuplicate(GhostEvidenceListType *, EvidenceNodeType*);
// PRINT STATEMENTS
void printHunter(HunterType*);
void printGhost(GhostType*);
void printGhostEvidenceList(GhostEvidenceListType*);
void printEvidence(EvidenceType*);
void printRoom(RoomType*);
void printRoomList(RoomListType*);
void printHunterList(HunterListType*);


void printBuilding(BuildingType*);

