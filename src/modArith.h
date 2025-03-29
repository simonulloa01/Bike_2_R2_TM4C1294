#ifndef MOD_ARITH_H
#define MOD_ARITH_H

#include "defs.h"

// Define the irreducible polynomial for modular operations. 
// In this case the polynomial is x^10162 + 1. Therefore, a 
// 1 in the LSB bit 0 and the MSB or bit 10163 (R_BITS).
#define IRREDUCIBLE_POLY 

void modInv(uint8_t *dst, const uint8_t *src);

void modMult(uint8_t *dst, const uint8_t *a, const uint8_t *b);

void modAdd(uint8_t *dst, const uint8_t *a, const uint8_t *b);

void polyMod(uint8_t *poly);


#endif