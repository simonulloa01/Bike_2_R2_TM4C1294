#include "bike2.h"

void keygen(PublicKey *pk, PrivateKey *sk)
{
    uint8_t h0[KEY_SIZE];
    uint8_t h1[KEY_SIZE];
    uint8_t h0_inv[KEY_SIZE];

    // todo
    randSample(h0);
    randSample(h1);

    // sk = h1 || h0
    memcpy(sk, h1, KEY_SIZE);
    memcpy(sk + KEY_SIZE, h0, KEY_SIZE);


    modInv(h0_inv, h0);
    modMult(pk, h0, h0_inv);
}

void encrypt(PublicKey *pk, Ciphertext *ct, SharedSecret *ss)
{

}

void decrypt(PrivateKey *sk, Ciphertext *ct, SharedSecret *ss)
{

}