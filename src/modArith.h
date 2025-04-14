#ifndef MOD_ARITH_H
#define MOD_ARITH_H

#include <stddef.h>
#include <math.h>
#include <string.h>
#include "defs.h"

void modInv(uint8_t *dst, const uint8_t *src, const bike2_params_t *params);

void modMult(uint8_t *dst, const uint8_t *a, const uint8_t *b, const uint32_t size);

void modAdd(uint8_t *dst, const uint8_t *a, const uint8_t *b, const uint32_t size);

void polyMod(uint8_t *dst, const uint8_t *a, const uint32_t size);


#endif