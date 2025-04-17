
#include "bike2-board.h"

void keygen_board(uint8_t *pk, uint8_t *sk, const bike2_params_t *params)
{
    // generate 2 sparse polynomials h0 and h1
    uint8_t h0[params->block_size];
    uint8_t h1[params->block_size];
    generate_sparse_polynomial(h0, params->block_size, params->row_weight / 2, params->code_length / 2);
    generate_sparse_polynomial(h1, params->block_size, params->row_weight / 2, params->code_length / 2);

    // invert h0 to get h0_inv
    uint8_t h0_inv[params->block_size];
    modInv(h0_inv, h0, params);

    // h = h1 * h0_inv
    uint8_t h[params->block_size];
    modMult(h, h1, h0_inv, params->block_size, params);

    // assembled the keys pk = (01000... to block size, h) and sk = (h0, h1)
    // copy pk
    memset(pk, 0, params->block_size * 2);
    memcpy(pk + params->block_size, h, params->block_size);
    // copy pk
    memset(sk, 0, params->block_size * 2);
    memcpy(sk, h0, params->block_size);
    memcpy(sk + params->block_size, h1, params->block_size);
    return;
}
/**
 * Encrypts the message using the public key and returns the ciphertext
 * @param pk the public key
 * @param ct OUT the ciphertext
 * @param ss OUT the encapsulation key (K)
 * @param params the parameters for the algorithm
 */
void encap_board(const uint8_t *pk, uint8_t *ct, uint8_t *ss, const bike2_params_t *params)
{
    uint8_t e0[params->block_size];
    uint8_t e1[params->block_size];
    // gen 2 sparse polynomials e0 and e1 with hamming weight t
    generate_sparse_polynomial(e0, params->block_size, params->target_error / 2, params->code_length / 2);
    generate_sparse_polynomial(e1, params->block_size, params->target_error / 2, params->code_length / 2);
    // ct = e0 + e1 * pk
    uint8_t e1_pk[params->block_size];
    modMult(e1_pk, e1, pk, params->block_size, params);
    modAdd(ct, e0, e1_pk, params->block_size);
    // K = Hash(e0 concat e1)
    uint8_t e[params->block_size * 2];
    memcpy(e, e0, params->block_size);
    memcpy(e + params->block_size, e1, params->block_size);
    hash(ss, e, params->block_size * 2);
}
/**
 * Decrypts the ciphertext using the secret key and returns the shared secret
 * @param sk the sparse private key (h0, h1)
 * @param ct the ciphertext
 * @param ss OUT the encapsulation key (K)
 * @param params the parameters for the algorithm
 */
void decap_board(const uint8_t *sk, const uint8_t *ct, uint8_t *ss, const bike2_params_t *params)
{
    uint8_t syndrome[params->block_size];
    uint8_t h0[params->block_size];
    uint8_t h1[params->block_size];
    // get the h0 and h1 from the sk
    memcpy(h0, sk, params->block_size);
    memcpy(h1, sk + params->block_size, params->block_size);

    // compute the syndrome
    modMult(syndrome, ct, sk, params->block_size, params);

    // decode the syndrome (dummy function)
    decode(ss, syndrome, params);
}

/**
 * Simulates a 348 bit (48 byte) hash of the input
 * @param output OUT the hash
 * @param input the byte array to hash
 * @param length the length of the input
 */
void hash(uint8_t *output, const uint8_t *input, const uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        output[i % 48] ^= input[i];
    }
}

/**
 * A dummy function placeholding the decode step for decaps_board
 * @param ss OUT the shared secret recovered from teh syndrome
 * @param syndome the syndrome to decode
 * @param params the parameters for the algorithm
 */
void decode(uint8_t *ss, const uint8_t *syndrome, const bike2_params_t *params)
{
    return;
}
