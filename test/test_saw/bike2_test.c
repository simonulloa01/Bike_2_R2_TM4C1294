#include <stdio.h>
#include <stdint.h>
#include <stdbool.h> 
#include <string.h>

// #define TEST_SAW

#ifdef TEST_SAW

#define R_SIZE 1271

// Functional Structs
typedef struct {
    uint16_t a;
    uint16_t b;
} UInt16Pair;

// Functions to test
uint8_t modAdd(const uint8_t *a, const uint8_t *b);
uint8_t modMult(const uint8_t *a, const uint8_t *b);
uint8_t polyMod(uint8_t *dst, const uint8_t *a, const uint32_t size);
UInt16Pair keyGen(uint8_t *pk, uint8_t *sk);

int main ()
{
    // Testing the modAdd function
    const uint8_t bin1 = 0b00010110;
    const uint8_t bin2 = 0b00100011;  
    uint8_t sum = 0;
    sum = modAdd(&bin1, &bin2);
    printf("Testing modAdd: The two values (%#x, %#x) --> %#x\n", bin1, bin2, sum);

    // Testing the modMult and mod function
    const uint8_t bin_1 = 22;
    const uint8_t bin_2 = 35;
    uint8_t prod = modMult(&bin_1, &bin_2);
    printf("Testing modMult and polyMod: The two values (%#x, %#x) --> %#x\n", bin_1, bin_2, prod);

    // Testing KeyGen function
    uint8_t h0 = 11;
    uint8_t h1 = 20;
    UInt16Pair keyGen_res = keyGen(&h0, &h1);
    printf("Testing keyGen: The two values (%#x, %#x) --> %#x\n", h0, h1, keyGen_res);

    return 0;
}


// Bike Specific Functions
// Polynomial Modular Arithemetic Functions
uint8_t modAdd(const uint8_t *a, const uint8_t *b)
{
    return *a ^ *b;
}

uint8_t modMult(const uint8_t *x, const uint8_t *y)
{
    const uint32_t size = 1;
    uint8_t dst = 0;
    // Do not perform arithmetic on polynomial's a and b if they are NULL.
    if ((x != NULL) && (y != NULL))
    {
        uint8_t product[size * 2];
        memset(product, 0, size * 2); // zero out product
        for (uint32_t byteIndex = 0; byteIndex < size; byteIndex++)
        {
            for (uint32_t bitIndex = 0; bitIndex < 8; bitIndex++)
            {
                bool bit = x[byteIndex] & (0x01 << bitIndex);
                if (bit)
                {
                    for (uint32_t i = 0; i < size; i++)
                    {
                        uint16_t productPart = 0x0000;
                        productPart |= y[i] << bitIndex;
                        product[i + byteIndex] ^= (uint8_t)(productPart & 0x00FF);
                        product[i + byteIndex + 1] ^= (uint8_t)(productPart >> 8);
                    }
                }
            }
        }

        return polyMod(&dst, product, size * 2);
    }
    return 0;
}

// Note: dst is expected to be an array of length R_SIZE
uint8_t polyMod(uint8_t *dst, const uint8_t *a, const uint32_t size)
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
                remainder[i - R_SIZE] = remainder[i - R_SIZE] ^ bitMask;
            }

            remainder[i] = remainder[i] & (~bitMask);
            bitMask = bitMask << 1;
        }
    }

    memcpy(dst, remainder, (size / 2));
    return *dst;
}


// We are comparing this function to the one in Cryptol. 
// In cryptol we provided the h0 and the h1 as these sparsely generated 
// functions. Therefore, here we will assume that they are provided for the 
// c implementation as well. 
UInt16Pair keyGen(uint8_t *h0, uint8_t *h1)
{
    // Return values to the keygen
    uint16_t pk = 0;
    uint16_t sk = 0;

    // the block size in bytes (r)
    int block_size = 1;

    // invert h0 to get h0_inv
    // Currently, the inverse is being implemented and tested. 
    // Therefore, we will place dummy data in its place to simplify 
    // testing. It can be refactored later to support full test. 
    uint8_t h0_inv[block_size];
    *h0_inv = 1; // dummy value

    // h = h1 * h0_inv -- where h is pk_prod
    uint8_t pk_prod;
    pk_prod = modMult(h1, h0_inv);

    // We use this prefix in the Cryptol implementation
    uint8_t prefix = 64; // simplified version to original 010000... value 

    // Assemble the key pk = (0b01000000 append pk_prod)
    pk = (prefix << 8) | pk_prod;

    // Assemble the key sk = (h0 append h1)
    sk = (*h0 << 8) | *h1;

    // The output of keygen is the pair of keys (pk, sk)
    // However, in Cryptol this is evaluated with the bytes reversed 
    // in a different Endianness. So here we flip the return values.
    // The return values on their own pk and sk match their Cryptol counterpart. 
    UInt16Pair ret[2] = {sk, pk}; // flipped to match the output on Cryptol

    return *ret;
}

#endif