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

//Entity types
typedef struct {
    char name[MAX_STR];
    struct RoomListType* connectedRooms;
    struct GhostEvidenceList* evidenceList;
    struct HunterType *hunters[MAX_HUNTERS];
    struct GhostType* ghost;
} RoomType;

typedef struct {
    struct GhostType* ghost;
    struct HunterType* hunters[MAX_HUNTERS];
    struct RoomListType* rooms;
} BuildingType;

typedef struct Ghost{
  GhostClassType *ghostType;
  struct RoomType *room;
  int boredomTimer; // initialize to BOREDOM_MAX
} GhostType;

typedef struct Hunter {
  struct RoomType *room;
  EvidenceClassType *evidence;
  struct GhostEvidenceList *personalEvidence;
  char name[MAX_STR];
  int fear;   // Init to zero
  int timer;  // Init to BOREDOM_MAX
} HunterType;

//NodeTypes
typedef struct RoomNode{
    struct RoomType* data;
    struct RoomNode* next;
} RoomNodeType;

typedef struct Evidence {
  EvidenceClassType *evidenceCategory;
  int readingData;
} EvidenceType;

typedef struct EvidenceNode{
    struct EvidenceType* data;
    struct EvidenceNode* next;
} EvidenceNodeType;

//LinkedListTypes
typedef struct GhostEvidenceList{
    struct EvidenceNode* head;
    struct EvidenceNode* tail;
} GhostEvidenceListType;

typedef struct RoomList{
    struct RoomNode* head;
    struct RoomNode* tail;
} RoomListType;

void populateRooms(BuildingType*);   // Populates the building with sample data for rooms