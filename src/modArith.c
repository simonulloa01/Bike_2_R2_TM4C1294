
#include <stddef.h>
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

void polyMod(uint8_t *poly)
{
    #if 0
    int degree = 5; // test value -- 10163 is actual real value 
    int pow = 0;
    uint8_t tmpPoly = poly;
    uint8_t result = 0;
    uint8_t irreduciblePoly = 0x22; // test value x^5 + x -- actual value is x^10163 + x

    // For modular reduction, we can perform the mod on each term individually.
    // Add each term together to form the final version of the polynomial.
    while (tmpPoly)
    {
        // When the power is less than the degree the result will be the
        // term that we are currently on -- no processing.  
        if (pow < degree)
        {
            result ^= (1 << pow);
        }
        // When pow and degree are equal, we can XOR the irreducible polynomial
        // with a polynomial with one term of degree pow. For example, if pow is 5
        // the polynomial will be x^5. 
        else if (pow == degree)
        {
            result ^= ((1 << pow) ^ irreduciblePoly);
        }
        // When pow is greater than degree we can multiply the irreduciblePoly
        // and the polynomial term that is higher than the degree in the field.
        // This makes it so that pow and degree are equivalent, allowing us to 
        // perform the operation we did eaerlier. 
        else if (pow > degree)
        {
            uint8_t tmpRes = 0; 
            uint8_t powPoly = (1 << pow);
            modMult(&tmpRes, &irreduciblePoly, &powPoly);
            result ^= ((1 << pow) ^ irreduciblePoly);

        }
        pow++;
    }
    *poly = result;
    #endif
    
}
