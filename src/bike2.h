
#ifndef BIKE_2_H
#define BIKE_2_H

#include <string.h>
#include "defs.h"
#include "modArith.h"
// #include "rng.h"

void keygen(uint8_t *pk, uint8_t *sk, const uint8_t* seed);

void encrypt(const uint8_t *pk, uint8_t *ct, uint8_t *ss, const uint8_t* seed);

bool decrypt(const uint8_t *sk, const uint8_t *ct, uint8_t *ss, const uint8_t* seed);

int hammingWeight(const uint8_t *poly);

#endif
