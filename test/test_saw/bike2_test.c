#include <stdio.h>
#include <stdint.h>
// #include "../../src/bike2.h"


// Functions to test
uint32_t modAdd(const uint32_t *a, const uint32_t *b);

// void keygen(uint8_t *pk, uint8_t *sk, const uint8_t *seed);
// void encrypt(const uint8_t *pk, uint8_t *ct, uint8_t *ss, const uint8_t *seed);
// bool decrypt(const uint8_t *sk, const uint8_t *ct, uint8_t *ss, const uint8_t *seed);


int main ()
{
    // Testing the modAdd function
    const uint32_t bin1 = 0b00010110;
    const uint32_t bin2 = 0b00100011;
    uint32_t sum = 0;
    sum = modAdd(&bin1, &bin2);
    printf("Testing modAdd: The two values (%#x, %#x) --> %#x\n", bin1, bin2, sum);
    return 0;
}

// Bike Specific Functions
// Polynomial Modular Arithemetic Functions
uint32_t modAdd(const uint32_t *a, const uint32_t *b)
{
    return *a ^ *b;
}

// void keygen(uint8_t *pk, uint8_t *sk, const uint8_t *seed)
// {
//     uint8_t h0[R_SIZE];
//     uint8_t h1[R_SIZE];
//     uint8_t h0_inv[R_SIZE];
//     aes_ctr_prf_state rngState = {0};

//     // init rng
//     init_aes_ctr_prf_state(&rngState, seed);

//     // randomly generate h0 and h1
//     randSample(h0, PUBLIC_KEY_WEIGHT, R_BITS, &rngState);
//     randSample(h1, PUBLIC_KEY_WEIGHT, R_BITS, &rngState);

//     // sk = h0 || h1
//     memcpy(sk, h0, R_SIZE);
//     memcpy(sk + R_SIZE, h1, R_SIZE);

//     // pk = h1 * h0^-1
//     modInv(h0_inv, h0);
//     modMult(pk, h1, h0_inv);
// }

// void encrypt(const uint8_t *pk, uint8_t *ct, uint8_t *ss, const uint8_t *seed)
// {
//     uint8_t e[N_SIZE];
//     aes_ctr_prf_state rngState = {0};

//     // init rng
//     init_aes_ctr_prf_state(&rngState, seed);

//     // randomly sample e with weight T
//     randSample(e, T, N_BITS, &rngState);

//     // split e into e0 and e1
//     uint8_t e0[R_SIZE];
//     uint8_t e1[R_SIZE];
//     memcpy(e0, e, R_SIZE);
//     memcpy(e1, e + R_SIZE, R_SIZE);

//     // ct = (e1*pk + e0)
//     uint8_t product[R_SIZE];
//     modMult(product, e1, pk);
//     modAdd(ct, product, e0);

//     // get shared secret
//     // todo

// }

// bool decrypt(const uint8_t *sk, const uint8_t *ct, uint8_t *ss, const uint8_t *seed)
// {
//     bool retVal = false;
//     uint8_t e[N_SIZE];
//     int hamWeight = 0;

//     //compute syndrome
//     //todo

//     //decode
//     //todo

//     //check hamming weight of e
//     hamWeight = hammingWeight(e, N_SIZE);

//     return retVal;
// }