#include "bike2.h"
#include "sampling.h"
void keygen(uint8_t *pk, uint8_t *sk, const uint8_t *seed)
{
    #if FALSE
    uint8_t h0[R_SIZE];
    uint8_t h1[R_SIZE];
    uint8_t h0_inv[R_SIZE];
    aes_ctr_prf_state rngState = {0};

    // init rng
    init_aes_ctr_prf_state(&rngState, seed);

    // randomly generate h0 and h1
    randSample(h0, PUBLIC_KEY_WEIGHT, R_BITS, &rngState);
    randSample(h1, PUBLIC_KEY_WEIGHT, R_BITS, &rngState);

    // sk = h0 || h1
    memcpy(sk, h0, R_SIZE);
    memcpy(sk + R_SIZE, h1, R_SIZE);

    // pk = h1 * h0^-1
    modInv(h0_inv, h0);
    modMult(pk, h1, h0_inv);
    #endif
}

void encrypt(const uint8_t *pk, uint8_t *ct, uint8_t *ss, const uint8_t *seed)
{

    uint8_t e[N_SIZE];
    
    //aes_ctr_prf_state rngState = {0};

    // init rng
    //Commented out to compile to device @cgrohs27 we need to fix this openssl doesn't work on this device
    //init_aes_ctr_prf_state(&rngState, seed);

    // randomly sample e with weight T 
    //Commented out to compile to device @cgrohs27 we need to fix this openssl doesn't work on this device
    //randSample(e, T, N_BITS, &rngState);

    memcpy(e, generate_sparse_polynomial(T, N_BITS), N_SIZE);
    // split e into e0 and e1
    uint8_t e0[R_SIZE];
    uint8_t e1[R_SIZE];
    memcpy(e0, e, R_SIZE);
    memcpy(e1, e + R_SIZE, R_SIZE);


    // ct = (e1*pk + e0)
    uint8_t product[R_SIZE];
    modMult(product, e1, pk);
    modAdd(ct, product, e0);

    // get shared secret
    // todo

}

bool decrypt(const uint8_t *sk, const uint8_t *ct, uint8_t *ss, const uint8_t *seed)
{
    #if 0
    bool retVal = false;
    uint8_t e[N_SIZE];
    int hamWeight = 0;

    //compute syndrome
    //todo

    //decode
    //todo

    //check hamming weight of e
    hamWeight = hammingWeight(&e);

    return retVal;
}

int hammingWeight(const uint8_t *poly)
{
    int weight = 0; 
    if (poly != NULL)
    {
        // While the polynomial has a valid value. 
        while (poly) 
        {
            // If the value is a 1 then we can increment the weight by 1. 
            if (poly & 1) 
            {
                weight++; 
            }
        }
    }
    return weight;
    #endif
    return false;
}
