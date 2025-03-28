#ifndef MOD_ARITH_H
#define MOD_ARITH_H

#include "defs.h"

// Define the irreducible polynomial for modular operations. 
// this will be a polynomial depicted as a binary number with a 
// 1 in the LSB bit and the MSB or bit 10163 (R_BITS).
#define IRREDUCIBLE_POLY 

void modInv(uint8_t *dst, const uint8_t *src);

void modMult(uint8_t *dst, const uint8_t *a, const uint8_t *b);

void modAdd(uint8_t *dst, const uint8_t *a, const uint8_t *b);

#endif