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

static double lnbino(size_t n, size_t t);
static double xlny(double x, double y);
static double lnbinomialpmf(size_t n, size_t k, double p, double q);
static double Euh_log(size_t n, size_t w, size_t t, size_t i);
static double iks(size_t r, size_t n, size_t w, size_t t);
static double counters_C0(size_t n, size_t d, size_t w, size_t S, size_t t,
                          double x);
static double counters_C1(size_t n, size_t d, size_t w, size_t S, size_t t,
                          double x);

int decode(uint8_t *e, uint8_t *syndrome, const uint8_t *h0, const uint8_t *h1, const bike2_params_t *params);
#endif