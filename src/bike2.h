#ifndef BIKE_2_H
#define BIKE_2_H

#include <string.h>
#include "defs.h"
#include "modArith.h"
#include "rng.h"

void keygen(uint8_t *pk, uint8_t *sk, const uint8_t* seed);

void encrypt(uint8_t *pk, uint8_t *ct, uint8_t *ss);

void decrypt(uint8_t *sk, uint8_t *ct, uint8_t *ss);

#endif