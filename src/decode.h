#ifndef BIKE2_DECODE_H
#define BIKE2_DECODE_H

#include "bike2-board.h"
#include "ring_buffer.h"
#include "sampling.h"

// decoding
#define TTL_COEFF0 0.45
#define TTL_COEFF1 1.1
#define TTL_SATURATE 5
#define max_iter 100
#define TTL_MAX (TTL_SATURATE + 1)
static inline int compute_ttl(int diff)
{
    int ttl = (int)(diff * TTL_COEFF0 + TTL_COEFF1);

    ttl = (ttl < 1) ? 1 : ttl;
    return (ttl > TTL_SATURATE) ? TTL_SATURATE : ttl;
}

int decode(uint8_t *ss, uint8_t *syndrome, const uint8_t *h0, const uint8_t *h1, const bike2_params_t *params);
#endif