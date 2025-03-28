#include "modArith.h"

void modInv(uint8_t *dst, const uint8_t *src)
{
    // todo
}

void modMult(uint8_t *dst, const uint8_t *a, const uint8_t *b)
{
    //
}

void modAdd(uint8_t *dst, const uint8_t *a, const uint8_t *b)
{
    // Do not perform arithmetic on polynomial's a and b if they are NULL. 
    if ((a != NULL) && (b != NULL))
    {
        // Adding the two binary polynomials is the equivalent of the XOR
        // operation. One of the reasons for this is because the inverse of
        // number is itself. For example, 1 plus 1 is 0, since the number 
        // is its own inverse.  
        *dst = *a ^ *b;
    }
}