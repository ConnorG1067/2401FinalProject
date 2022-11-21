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

typedef struct Hunter {
  struct RoomType *room;
  EvidenceClassType *evidence;
  struct GhostEvidenceList *personalEvidence;
  char name[MAX_STR];
  int fear;   // Init to zero
  int timer;  // Init to BOREDOM_MAX
} HunterType;

//Entity types
typedef struct Room{
    char name[MAX_STR];
    struct RoomListType* connectedRooms;
    struct GhostEvidenceList* evidenceList;
    HunterType *hunters[MAX_HUNTERS];
    int hunterListSize;
    struct GhostType* ghost;
} RoomType;

typedef struct Ghost{
  GhostClassType *ghostType;
  struct RoomType *room;
  int boredomTimer; // initialize to BOREDOM_MAX
} GhostType;



typedef struct Evidence {
  EvidenceClassType *evidenceCategory;
  int readingData;
} EvidenceType;

typedef struct EvidenceNode{
    EvidenceType* data;
    struct EvidenceNode* next;
} EvidenceNodeType;

//NodeTypes
typedef struct RoomNode{
    RoomType* data;
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

typedef struct {
    GhostType* ghost;
    HunterType* hunters[MAX_HUNTERS];
    int hunterListSize;
    RoomListType* rooms;
} BuildingType;

void populateRooms(BuildingType*);   // Populates the building with sample data for rooms
void initBuilding(BuildingType*);
void initRoom(RoomType*, char*);
void initRoomList(RoomListType *);

void initGhostList(GhostEvidenceListType *);
void initGhost(GhostClassType *, RoomType*, GhostType *);


