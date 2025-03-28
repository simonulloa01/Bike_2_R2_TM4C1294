#include "bike2.h"

void keygen(uint8_t *pk, uint8_t *sk, const uint8_t* seed)
{
    uint8_t h0[PUBLIC_KEY_SIZE];
    uint8_t h1[PUBLIC_KEY_SIZE];
    uint8_t h0_inv[PUBLIC_KEY_SIZE];
    aes_ctr_prf_state rngState = {0};

    //init rng
    init_aes_ctr_prf_state(&rngState, seed);

    // randomly generate h0 and h1
    randSample(h0, PUBLIC_KEY_WEIGHT, PUBLIC_KEY_BITS, &rngState);
    randSample(h1, PUBLIC_KEY_WEIGHT, PUBLIC_KEY_BITS, &rngState);

    // sk = h0 || h1
    memcpy(sk, h0, PUBLIC_KEY_SIZE);
    memcpy(sk + PUBLIC_KEY_SIZE, h1, PUBLIC_KEY_SIZE);

    // pk = h1 * h0^-1
    modInv(h0_inv, h0);
    modMult(pk, h1, h0_inv);
}

void encrypt(uint8_t *pk, uint8_t *ct, uint8_t *ss)
{

}

void decrypt(uint8_t *sk, uint8_t *ct, uint8_t *ss)
{

}