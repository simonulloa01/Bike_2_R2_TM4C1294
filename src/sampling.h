#ifndef GENERATOR_H
#define GENERATOR_H
#include <stdint.h>
#include <stddef.h>

void generate_sparse_polynomial(
    uint8_t *out_bits,
    size_t out_size_bytes,
    uint32_t weight,
    uint32_t len_bits);

uint32_t get_hamming_weight(const uint8_t *poly, size_t len_bits);
#endif
