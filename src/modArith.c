
#include "modArith.h"

void modMult(uint8_t *dst, const uint8_t *a, const uint8_t *b, const uint32_t size, const bike2_params_t *params)
{
    uint32_t product_bits = 2 * size;
    uint32_t product_bytes = (product_bits + 7u) / 8u;

    
    uint8_t product[product_bytes];
    memset(product, 0, product_bytes);

    //foreach bit
    for (uint32_t i = 0; i < size; i++)
    {
        //check if bit i in 'a' is set 
        uint32_t a_byte = i / 8;
        uint32_t a_bit = i % 8;
        bool a_is_set = (a[a_byte] & (1u << a_bit)) != 0;

        if (a_is_set)
        {
            // For each bit j of b(x):
            for (uint32_t j = 0; j < size; j++)
            {
                //check if bit j in 'b' is set
                uint32_t b_byte = j / 8;
                uint32_t b_bit = j % 8;
                bool b_is_set = (b[b_byte] & (1u << b_bit)) != 0;

                if (b_is_set)
                {
                    uint32_t sum = i + j; // exponent
                    uint32_t sum_byte = sum / 8;
                    uint32_t sum_bit = sum % 8;
                    product[sum_byte] ^= (uint8_t)(1u << sum_bit);
                }
            }
        }
    }

    //reduce
    polyMod(dst, product, product_bits, params);
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

// size of the polynomial in bits
void polyMod(uint8_t *dst, const uint8_t *a, const uint32_t size, const bike2_params_t *params)
{
    // r = exponent for x^r -1
    const uint32_t r = params->block_size_bits;
    
    const uint32_t a_bytes = (size + 7u) / 8u;
    const uint32_t r_bytes = (r + 7u) / 8u;

    // Local copy so we can modify bits in place
    uint8_t remainder[a_bytes];
    memset(remainder, 0, a_bytes);
    memcpy(remainder, a, a_bytes);

    //For each set bit i >= r, fold it to (i - r) from highest bit
    for (int i = (int)size - 1; i >= (int)r; i--)
    {
        uint32_t bytePos = (uint32_t)i / 8u;
        uint32_t bitPos = (uint32_t)i % 8u;
        uint8_t mask = (uint8_t)(1u << bitPos);

        //check if set
        if (remainder[bytePos] & mask)
        {
            // clear bit i
            remainder[bytePos] &= (uint8_t)(~mask);

            // toggle bit (i - r)
            int j = i - (int)r;                // new exponent
            uint32_t jb = (uint32_t)j / 8u;    // which byte
            uint32_t jbpos = (uint32_t)j % 8u; // which bit in that byte
            uint8_t jmask = (uint8_t)(1u << jbpos);

            remainder[jb] ^= jmask;
        }
    }


    memset(dst, 0, r_bytes);
    const uint32_t toCopy = (a_bytes < r_bytes) ? a_bytes : r_bytes;
    memcpy(dst, remainder, toCopy);

    // if r is not a multiple of 8 mask off the top bits in the last byte
    if (r % 8u != 0u)
    {
        uint8_t topMask = (uint8_t)((1u << (r % 8u)) - 1u);
        dst[r_bytes - 1] &= topMask;
    }
}
void modInv(uint8_t *dst, const uint8_t *src, const bike2_params_t *params)
{
}
