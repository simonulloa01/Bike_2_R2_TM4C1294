
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
    // Step 0 - Set up parameters: 
    // Each parameter will be represented a two-element array of 1271 bytes each. 
    const uint8_t ct0, ct1, h0, h1 [params->block_size];  
    uint8_t ss0, ss1[params->block_size]; // secret key

    // Each variable should have half of the parameter passed in. 
    // ciphertext passed in 
    *ct0 = ct;
    *ct1 = ct + params->block_size;
    // private sparse key passed in 
    *h0 = sk;
    *h1 = sk + params->block_size;
    // shared secret key output.  
    *ss0 = ss;
    *ss1 = ss + params->block_size;
    
    // Step 1 - Compute Syndrome: 
    // In BIKE2-CPA syndrome must become a zero-vector
    // BIKE-2 syndrome: s = c0*h0
    uint8_t syndrome[params->block_size_bits]; // array of 10163 bytes
    modMult(syndrome, h0, ct0, params->block_size, params);

    // Convert syndrome 1271 bytes to bit-array of 1271 size. 
    


    // Step 3 - Transpose the syndrome
    // transpose(col, row) // col represents final syndrome and row is the 10163 binary syndrome from prev step.
    // Ex.
    // row = {A, B, C, D, E}
    // col[0] = row[0] = A
    // col[1] = row[4] = E
    // col[2] = row[3] = D
    // col[3] = row[2] = C
    // col[4] = row[1] = B

    // decode the syndrome (dummy function)
    decode(ss, syndrome, params);

    // Step 6 - Check hamming weight, if it does not match the pre-defined value
    // (134) then we can return early, otherwise continue to next step.

    // Step 7 - Generate shared secret 
    // res = get_ss_cpa(l_ss, eBytes);                CHECK_STATUS(res);
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
 * @param ss OUT the shared secret recovered from the syndrome
 * @param syndome the syndrome to decode
 * @param params the parameters for the algorithm
 */
void decode(uint8_t *ss, const uint8_t *syndrome, const bike2_params_t *params)
{
    // Step 4 - Decode using decode_1sr_round from BIKE Reference 
    // rc = decode_1st_round(e, syndrome.raw, h0_compact, h1_compact, u);

    // Step 5 - Decode returns an int that indicates whether it was successful. 
    // If it returns 0 then it was successful otherwise, decode portion failed.
    // Convert array of bits to array of bytes

    return;
}
