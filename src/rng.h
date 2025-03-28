#ifndef RNG_H
#define RNG_H

#include "defs.h"

uint32_t randModLen(uint32_t len, uint32_t* rng_state);

void randSample(uint8_t *result, uint32_t weight, uint32_t len, uint32_t* rng_state);

#endif