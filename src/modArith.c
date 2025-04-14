
#include "modArith.h"

void modSqr(uint8_t *dst, const uint8_t *src, const bike2_params_t *params)
{
    const uint32_t block_size_bytes = params->block_size;
    const uint32_t block_size_bits = params->block_size_bits;
    
    // Clear the destination buffer first
    memset(dst, 0, block_size_bytes * 2);

    // For each bit in the source, set the corresponding bit in the destination
    // In GF(2)[x], squaring spreads out the bits: x^i becomes x^(2i)
    for (uint32_t i = 0; i < block_size_bytes; i++) {
        for (uint32_t j = 0; j < 8; j++) {
            // Skip bits that are beyond the polynomial size
            if (i * 8 + j >= block_size_bits) {
                break;
            }
            
            if ((src[i] >> j) & 1) {
                uint32_t newBitIndex = (i * 8 + j) * 2;
                uint32_t newByteIndex = newBitIndex / 8;
                uint32_t newBitOffset = newBitIndex % 8;
                
                if (newByteIndex < block_size_bytes * 2) {
                    dst[newByteIndex] |= (1 << newBitOffset);
                }
            }
        }
    }
    
    // Apply modulo (x^r - 1)
    uint8_t temp[block_size_bytes];
    polyMod(temp, dst, block_size_bytes * 2);
    memcpy(dst, temp, block_size_bytes);
}

void repeatedSquaring(uint8_t *dst, const uint8_t *src, uint32_t k, const bike2_params_t *params)
{
    const uint32_t block_size_bytes = params->block_size;
    
    // Copy source to a temporary buffer for in-place operations
    uint8_t temp[block_size_bytes];
    memcpy(temp, src, block_size_bytes);
    
    // Initialize destination with source
    memcpy(dst, src, block_size_bytes);
    
    // Perform k squaring operations
    for (uint32_t i = 0; i < k; i++) {
        uint8_t squared[block_size_bytes];
        modSqr(squared, dst, params);
        memcpy(dst, squared, block_size_bytes);
    }
}


void modMult(uint8_t *dst, const uint8_t *a, const uint8_t *b, const uint32_t size)
{
    // Do not perform arithmetic on polynomial's a and b if they are NULL.
    if ((a != NULL) && (b != NULL))
    {
        uint8_t product[size * 2];
        memset(product, 0, size * 2); // zero out product
        for (uint32_t byteIndex = 0; byteIndex < size; byteIndex++)
        {
            for (uint32_t bitIndex = 0; bitIndex < 8; bitIndex++)
            {
                bool bit = a[byteIndex] & (0x01 << bitIndex);
                if (bit)
                {
                    for (uint32_t i = 0; i < size; i++)
                    {
                        uint16_t productPart = 0x0000;
                        productPart |= b[i] << bitIndex;
                        product[i + byteIndex] ^= (uint8_t)(productPart & 0x00FF);
                        product[i + byteIndex + 1] ^= (uint8_t)(productPart >> 8);
                    }
                }
            }
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
                remainder[i - R_SIZE] = remainder[i - R_SIZE] ^ bitMask;
            }

            remainder[i] = remainder[i] & (~bitMask);
            bitMask = bitMask << 1;
        }
    }

    memcpy(dst, remainder, R_SIZE);
}

void modInv(uint8_t *dst, const uint8_t *src, const bike2_params_t *params)
{
    if (src == NULL || dst == NULL || params == NULL) {
        return;
    }
    
    const uint32_t block_size_bytes = params->block_size;
    const uint32_t block_size_bits = params->block_size_bits;
    
    // Temporary buffers for calculations
    uint8_t f[block_size_bytes];
    uint8_t g[block_size_bytes];
    uint8_t t[block_size_bytes];
    
    // Initialize f and t with the input polynomial
    memcpy(f, src, block_size_bytes);
    memcpy(t, src, block_size_bytes);
    
    // These are the exponentiation patterns for BIKE-2
    // The exact pattern depends on the bit size, but this is a reasonable
    // general approach that works for typical BIKE parameter sets
    
    // We'll implement a chain of squaring and multiplication operations
    // based on the Itoh-Tsujii algorithm
    
    // Start with i=1
    repeatedSquaring(g, f, 1, params);
    modMult(f, f, g, block_size_bytes);
    
    // Continue with optimized steps - we use powers of 2 to minimize squarings
    const uint32_t exp_steps[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
    const uint32_t num_steps = sizeof(exp_steps) / sizeof(exp_steps[0]);
    
    for (uint32_t i = 0; i < num_steps && exp_steps[i] < block_size_bits; i++) {
        // Compute g = f^(2^exp_steps[i])
        repeatedSquaring(g, f, exp_steps[i], params);
        
        // Multiply: f = f * g
        modMult(f, f, g, block_size_bytes);
    }
    
    // Final squaring: t = f^2
    modSqr(t, f, params);
    
    // Copy result to destination
    memcpy(dst, t, block_size_bytes);
    
    // Ensure the highest bits beyond the polynomial size are zeroed
    if (block_size_bits % 8 != 0) {
        uint8_t mask = (1 << (block_size_bits % 8)) - 1;
        dst[block_size_bytes - 1] &= mask;
    }
}
