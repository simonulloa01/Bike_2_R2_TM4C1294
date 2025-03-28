#include "rng.h"
#include <string.h>

uint32_t randModLen(uint32_t len, uint32_t* rng_state)
{
    //todo
    return 0;
}


void randSample(uint8_t* result, uint32_t weight, uint32_t len, uint32_t* rng_state)
{
    uint32_t randomPosition;
    uint64_t ctr = 0;

    //make sure the result is all zeros
    memset(result, 0, len);

    // set random bits in result until the weight matches
    do
    {
        //get random position
        randomPosition = randModLen(len, rng_state);

        // check that this bit has not been set yet
        uint32_t arrayIndex = randomPosition/8;
        uint8_t bitIndex = randomPosition % 8; 
        if((result[arrayIndex] >> bitIndex) & 0x01)
        {
            //set bit
            result[arrayIndex] |= 0x01 << (bitIndex);
            ctr++;
        }
    } while (ctr != weight);
}