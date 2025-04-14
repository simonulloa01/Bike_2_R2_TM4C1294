#include "sampling.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void generate_sparse_polynomial(
    uint8_t  *out_bits,
    size_t    out_size_bytes,
    uint32_t  weight,
    uint32_t  len_bits
)
{
    
    size_t needed_bytes = (len_bits + 7u) / 8u;


    if (out_size_bytes < needed_bytes) {
        printf("Error: output buffer is too small\n");
        exit(EXIT_FAILURE);
        return;
    }
    
    memset(out_bits, 0, needed_bytes);


    uint32_t count = 0;
    while (count < weight) {
        
        uint32_t bit_index = (uint32_t)(rand() % len_bits);

        uint32_t byte_index = bit_index / 8;
        uint32_t bit_position = bit_index % 8;

        //set a bit if it is not already set
        if ((out_bits[byte_index] & (1u << bit_position)) == 0) {
            out_bits[byte_index] |= (1u << bit_position);
            count++;
        }
        
    }
}
uint32_t get_hamming_weight(const uint8_t *poly, size_t len_bits)
{
    uint32_t count = 0;
    for (uint32_t i = 0; i < len_bits; i++)
    {
        uint32_t byte_index = i / 8;
        uint32_t bit_position = i % 8;
        if ((poly[byte_index] & (1u << bit_position)) != 0)
        {
            count++;
        }
    }
    return count;
}

