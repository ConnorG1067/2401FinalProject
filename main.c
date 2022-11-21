#include "defs.h"

int main(int argc, char *argv[])
{
    // Initialize a random seed for the random number generators
    srand(time(NULL));

    // You may change this code; this is for demonstration purposes
<<<<<<< HEAD
    //BuildingType building;
    //initBuilding(&building);
=======
    BuildingType building;
    initBuilding(&building);
>>>>>>> f4cbc6a6c70c40a6603fef817084a72aa692e605
    //populateRooms(&building);

    char names[MAX_HUNTERS][MAX_STR];

    for(int i = 0; i < MAX_HUNTERS; i++) {
<<<<<<< HEAD
        scanf("INPUT FUCKING NAME YOU FUCKING STUPID RETARD: %s", names[i]);
    }
=======
        printf("INPUT FUCKING NAME YOU FUCKING STUPID RETARD: ");
        scanf("%s", names[i]);
    }
    
>>>>>>> f4cbc6a6c70c40a6603fef817084a72aa692e605

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
