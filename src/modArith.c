
#include "modArith.h"

void modInv(uint8_t *dst, const uint8_t *src, const uint32_t size)
{
    // todo
}

void modMult(uint8_t *dst, const uint8_t *a, const uint8_t *b, const uint32_t size)
{
    // Do not perform arithmetic on polynomial's a and b if they are NULL.
    if ((a != NULL) && (b != NULL))
    {
        uint8_t product[size * 2];
        for (uint32_t byteIndex = 0; byteIndex < size; byteIndex++)
        {
            uint16_t productPart = 0x0000;
            for (uint32_t bitIndex = 0; bitIndex < 8; bitIndex++)
            {
                bool bit = a[byteIndex] & (0x01 << bitIndex);
                if (bit)
                {
                    productPart |= b[byteIndex] << bitIndex;
                }
            }
            product[byteIndex] |= (uint8_t)(productPart & 0x00FF);
            product[byteIndex + 1] |= (uint8_t)(productPart >> 8);
        }

        polyMod(dst, product, size * 2);
    }
}

void modAdd(uint8_t *dst, const uint8_t *a, const uint8_t *b, const uint32_t size)
{

    // Do not perform arithmetic on polynomial's a and b if they are NULL.
    if ((a != NULL) && (b != NULL))
    {
        // Adding the two binary polynomials is the equivalent of the XOR
        // operation. One of the reasons for this is because the inverse of
        // number is itself. For example, 1 plus 1 is 0, since the number
        // is its own inverse.
        for (uint32_t i = 0; i < size; i++)
        {
            dst[i] = a[i] ^ b[i];
        }
    }
}

// Note: dst is expected to be an array of length R_SIZE
void polyMod(uint8_t *dst, const uint8_t *a, const uint32_t size)
{
    // copy over the operand and it will be worked down to the remainder
    uint8_t remainder[size];
    memcpy(remainder, a, size);

    // loop down to the 
    for (uint32_t i = size - 1; i >= R_SIZE; i--)
    {
        uint8_t bitMask = 0x01;
        while (remainder[i] != 0)
        {
            bool bit = remainder[i] & bitMask;
            
            if (bit)
            {
                remainder[i-R_SIZE] = remainder[i-R_SIZE] ^ bitMask;
            }

            remainder[i] = remainder[i] & (~bitMask);
            bitMask = bitMask << 1;
        }
    }

    memcpy(dst, remainder, R_SIZE);
}