#if false
#ifndef RNG_H
#define RNG_H

#include <string.h>
#include <openssl/aes.h>
#include "defs.h"

// custom unsigned 128 bit type
typedef struct uint128_s
{
    union
    {
        uint8_t bytes[16];
        uint32_t dwords[4];
        uint64_t qwords[2];
    };
} uint128_t;

typedef struct aes_ctr_prf_state_s
{
    uint128_t ctr;
    uint128_t buffer;
    AES_KEY   key;
    uint8_t   pos;
} aes_ctr_prf_state;

void perform_aes(uint8_t* ciphertext, aes_ctr_prf_state* rngState);

void aes_ctr_prf(uint8_t* a, aes_ctr_prf_state* rngState, const uint32_t len);

uint8_t bit_scan_reverse(uint64_t val);

uint32_t randModLen(const uint32_t len, aes_ctr_prf_state* rngState);

void randSample(uint8_t *result, const uint32_t weight, const uint32_t len, aes_ctr_prf_state* rngState);

void init_aes_ctr_prf_state(aes_ctr_prf_state* rngState, const uint8_t* seed);

#endif
#endif