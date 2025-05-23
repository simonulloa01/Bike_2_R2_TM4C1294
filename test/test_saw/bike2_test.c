#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h> 
#include <string.h>
#include <math.h>

// #define TEST_SAW

#ifdef TEST_SAW

#define R_SIZE 1271

// Functional Structs
typedef struct {
    uint16_t a;
    uint16_t b;
} UInt16Pair;

// Helper 
int getBit(const uint8_t *poly, int i);
void set_bit(uint8_t* poly, size_t degree, int coeff);
int degree(unsigned int n);
void increaseExponent(uint8_t **poly, int degree, int n);

// Functions to 
uint8_t karatsuba(const uint8_t *poly1, const uint8_t *poly2, int poly1_deg, int poly2_deg);
uint8_t modAdd(const uint8_t *a, const uint8_t *b);
uint8_t modMult(const uint8_t *a, const uint8_t *b);
uint8_t polyMod(uint8_t *dst, const uint8_t *a, const uint32_t size);
UInt16Pair keyGen(uint8_t *pk, uint8_t *sk);

int main ()
{
    // Testing the modAdd function
    const uint8_t bin1 = 0b00010011;
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

int getBit(const uint8_t *poly, int i)
{
    return (poly[i / 8] >> (i % 8)) & 1;
}

// poly: Pointer to the uint8_t array representing the polynomial.
// degree: The degree (position) of the term to set or clear.
// coeff: The value to set at the specified degree (0 or 1).
void set_bit(uint8_t* poly, size_t degree, int coeff) 
{
    size_t byte_index = degree / 8;
    uint8_t bit_mask = 1 << (degree % 8);

    if (coeff) 
    {
        poly[byte_index] |= bit_mask;  // Set the bit
    } 
    else
    {
        poly[byte_index] &= ~bit_mask; // Clear the bit
    }
}

int degree(unsigned int n) 
{
    if (n == 0) 
    {
        return -1; // Undefined for polynomials with value 0.
    }
    return (int)log2(n);
}

void increaseExponent(uint8_t **poly, int degree, int n) 
{
    // Update the degree of the polynomial
    degree += n;

    // Allocate memory for the new coefficients array
    uint8_t *new_poly = (uint8_t *)calloc(degree + 1, sizeof(uint8_t));
    if (new_poly == NULL) 
    {
        perror("Failed to allocate memory");
        exit(1);
    }

    // Copy existing coefficients to the new array
    memcpy(new_poly + n, *poly, ((degree - n) + 1) * sizeof(uint8_t));

    // Free the old coefficients array and update the pointer
    free(*poly);
    *poly = new_poly;
}


// Optimized Polynomial Modular Multiplication Operation.
uint8_t karatsuba(const uint8_t *poly1, const uint8_t *poly2, int poly1_deg, int poly2_deg)
{
    // Step 0 -- define a base case 
    // determine a max degree of polynomial in which we decide  to use our 
    // naive multiplication approach which is modMult
    // things we need: 
    // - degree of provided polynomials
    // - call modMult and return product on provided polynomials.
    if (poly1_deg <= 1 || poly2_deg <= 1)
    {
        return modMult(poly1, poly2);
    }

    uint8_t *res = calloc(poly1_deg + poly2_deg, sizeof(uint8_t));

    // Since this is the divide and conquer method lets find the degree of
    // half of the poly1.
    int half = floor(poly1_deg / 2);

    // Step 1 -- Define polynomials of half sizes to represent the 
    // upper and lower parts of the provided polynomials. Note that the
    // polynomials will each be initialized to 0.
    int A0_size = ((half - 1) + 7) / 8; 
    int A1_size = ((poly1_deg - half) + 7) / 8; 
    int B0_size = A0_size; 
    int B1_size = A1_size; 

    // Initialize the split polynomials.
    uint8_t *A0 = calloc(A0_size, sizeof(uint8_t));
    uint8_t *A1 = calloc(A1_size, sizeof(uint8_t));
    uint8_t *B0 = calloc(B0_size, sizeof(uint8_t));
    uint8_t *B1 = calloc(B1_size, sizeof(uint8_t));

    // Define upper polys to properly perform memcpy.
    uint8_t upperPoly1[A1_size];
    uint8_t upperPoly2[B1_size];

    // Step 2 -- Set A0, A1, B0, B1 arrays.
    // Set A0 to the lower half of poly1.
    // Set A1 to the upper half of poly1.
    // Set B0 to the lower half of poly2.
    // Set B1 to the upper half of poly2.
    for(int i = 0; i < half; ++i) 
    {
        set_bit(A0, i, getBit(poly1, i));
        set_bit(B0, i, getBit(poly2, i));
        set_bit(A1, i, getBit(poly1, i + half));
        set_bit(B1, i, getBit(poly2, i + half));

        if(degree(*poly1) % 2 == 1 && i + 1 == half) 
        {
            set_bit(A1, i, getBit(poly1, i + half));
            set_bit(B1, i, getBit(poly2, i + half));
        }
    }

    // Initialize polynomials used to hold the product of the multiplication operations. 
    int u_size = half * 2;
    uint8_t *U = calloc(u_size, sizeof(uint8_t));
    int z_size = ((poly1_deg - half - 1) + (poly2_deg - half - 1));
    uint8_t *Z = calloc(((poly1_deg - half - 1) + (poly2_deg - half - 1)), sizeof(uint8_t));

    // Get the max degree of the two polys to allocate sufficient space for helper poly.
    int maxA0A1 = fmax(degree(*A0), degree(*A1));
    uint8_t *A0_and_A1 = calloc(maxA0A1, sizeof(uint8_t));

    int maxB0B1 = fmax(degree(*B0), degree(*B1));
    uint8_t *B0_and_B1 = calloc(maxB0B1, sizeof(uint8_t));
    
    int y_size = maxA0A1 + maxB0B1;
    uint8_t *Y = calloc(y_size, sizeof(uint8_t));

    int w_max_size = y_size;

    if (u_size > w_max_size)
    {
        w_max_size = u_size;
    }
    if (z_size > w_max_size)
    {
        w_max_size = z_size;
    }

    uint8_t *W = calloc(w_max_size, sizeof(uint8_t)); 

    // Step 3 -- Compute the main 3 multiplication operations of Karatsuba.
    // Compute U = A0 * B0
    *U = karatsuba(A0, B0, half, half);

    // Compute Z = A1 * B1
    *Z = karatsuba(A1, B1, poly1_deg - half - 1, poly2_deg - half - 1);

    // Compute Y = (A0 + A1) * (B0 + B1)
    *A0_and_A1 = modAdd(A0, A1);
    *B0_and_B1 = modAdd(B0, B1);
    *Y = karatsuba(A0_and_A1, B0_and_B1, half, half);

    // Compute W = Y - U - Z 
    // In GF(2) subtraction is the same operation as addition
    *W = modAdd(Y, U);
    *W = modAdd(W, Z);

    increaseExponent(&W, degree(*W), half);
    increaseExponent(&Z, degree(*Z), poly1_deg);

    *res = modAdd(U, W);
    *res = modAdd(res, Z);

    // Free the split polynomials.
    free(A0); 
    free(A1); 
    free(B0); 
    free(B1);
    free(U); 
    free(Z); 
    free(A0_and_A1); 
    free(B0_and_B1); 
    free(Y); 
    free(W);

    return *res;
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