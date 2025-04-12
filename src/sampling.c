#include "sampling.h"
#include <stdlib.h>


uint8_t* generate_sparse_polynomial(const uint32_t weight, const uint32_t len) {
    
    // calculate number of bytes needed (ceiling of len/8)
    uint8_t* polynomial = (uint8_t*)calloc(len, 1);
    if (polynomial == NULL) {
        return NULL; 
    }
    
    uint32_t i = 0;
    while (i < weight) {
        //TODO: not secure fix later
        uint32_t bit_index = rand() % len;
        
        
        uint32_t byte_index = bit_index / 8;
        uint32_t bit_position = bit_index % 8;
        
        //check if bit is set
        if ((polynomial[byte_index] & (1 << bit_position)) == 0) {
            polynomial[byte_index] |= (1 << bit_position);
            i++;
        }
    }
    
    return polynomial;
}

