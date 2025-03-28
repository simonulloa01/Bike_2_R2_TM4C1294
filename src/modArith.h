#ifndef MOD_ARITH_H
#define MOD_ARITH_H

#include "defs.h"

void modInv(uint8_t *dst, const uint8_t *src);

void modMult(uint8_t *dst, const uint8_t *a, const uint8_t *b);

void modAdd(uint8_t *dst, const uint8_t *a, const uint8_t *b);

#endif