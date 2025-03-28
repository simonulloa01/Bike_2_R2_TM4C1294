#include "modArith.h"

void modInv(uint8_t *dst, const uint8_t *src)
{
    // todo
}

void modMult(uint8_t *dst, const uint8_t *a, const uint8_t *b)
{
    // Do not perform arithmetic on polynomial's a and b if they are NULL. 
    if ((a != NULL) && (b != NULL))
    {
        // Perform polynomial multiplication 
        uint8_t result = 0;
        uint8_t shift = 0;
        uint8_t poly1 = *a;
        uint8_t poly2 = *b;
        
        // While the polynomial still has valid value check each term.
        while (poly2) 
        {
            if (poly2 & 1) 
            {
                result ^= (poly1 << shift);
            }
            shift++;
            poly2 >>= 1;
        }
        
        *dst = result;
        // Perform polynomial reduction -- by the irreducible polynomial.
        // TODO
    }

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