
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
    uint8_t syndrome[params->block_size_bits] = {0};
    computeSyndrome(&syndrome, ct0, ct1, h0, h1, params);

    // Parameters necessary for decode.
    uint8_t e[params->block_size_bits*2] = {0};
    // In BIKE2-CPA syndrome must become a zero-vector
    uint32_t u = 0;
    
    // decode the syndrome (dummy function)
    decode(e, syndrome, h0, h1, u, params);
    

    // Step 5 - Decode returns an int that indicates whether it was successful. 
    // If it returns 0 then it was successful otherwise, decode portion failed.
    // Convert array of bits to array of bytes

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

void computeSyndrome(uint8_t *syndrome, uint8_t *ct0, uint8_t *ct1,
                     uint8_t *h0, uint8_t *h1, const bike2_params_t *params)
{
    uint8_t syndrome[params->block_size] = 0; 
    uint8_t syndrome_tmp[params->block_size_bits] = 0;
    uint8_t syndrome0 [params->block_size] = 0; 
    
    // BIKE-2 syndrome0: s = c0*h0
    modMult(&syndrome0, h0, ct0, params->block_size, params); 
    
    //Store the syndrome in a bit array
    // Convert syndrome_tmp 10163 bytes to bit-array of 1271 size. 
    // byteToBin(syndrome_tmp, syndrome0, block_size_bits); // size -> 10163

    // Step 3 - Transpose the syndrome
    // transpose(syndrome->raw, s_tmp_bytes); // also pass in size of 10163 block_size_bits
    transpose(syndrome, syndrome_tmp); // this will be final syndrome

    
}

void getCol(uint8_t *h_col, uint8_t *h_row)
{
    if (h_row[0] == 0)
    {
        h_col[0] = 0;

        for (uint32_t i = 1; i < PUBLIC_KEY_WEIGHT; i++)
        {
            // set indices in increasing order:
            h_col[i] = R_BITS - h_row[PUBLIC_KEY_WEIGHT-i];
        }
    } 
    else
    {
        for (uint32_t i = 0; i < PUBLIC_KEY_WEIGHT; i++)
        {
            // set indices in increasing order:
            h_col[i] = R_BITS - h_row[PUBLIC_KEY_WEIGHT-1-i];
        }
    }
}

/**
 * Simulates a 348 bit (48 byte) hash of the input
 * @param output OUT the hash
 * @param input IN byte array to hash
 * @param length the length of the input
 */
void transpose(uint8_t *col, uint8_t *row, const uint8_t size)
{
    // transpose(col, row, size) 
    // col represents final syndrome and row is the 10163 binary syndrome from prev step.
    // Ex.
    // row = {A, B, C, D}
    // col[0] = row[0] = A
    // col[2] = row[3] = D
    // col[3] = row[2] = C
    // col[4] = row[1] = B
    return;
}

void computeUnsatCounter(uint8_t *unsatCounter,
                         const uint8_t *syndrome,
                         const uint8_t *h0,
                         const uint8_t *h1)
{






    uint8_t unsatCounter2[N_BITS*2] = {0};
    uint32_t h1_compact2[PUBLIC_KEY_WEIGHT] = {0};

    for (uint32_t i = 0; i < PUBLIC_KEY_WEIGHT; i++)
    {
        h1_compact2[i] = N_BITS + h1[i];
    }

    for (uint32_t i = 0; i < R_BITS; i++)
    {
        if (!syndrome[i])
        {
            continue; 
        }

        for (uint32_t j = 0; j < PUBLIC_KEY_WEIGHT; j++)
        {
            unsatCounter2[h0[j] + i]++;
            unsatCounter2[h1_compact2[j] + i]++;
        }
    }

    for (uint32_t i = 0; i < R_BITS; i++)
    {
        unsatCounter[i] = unsatCounter2[i] + unsatCounter2[R_BITS+i];
        unsatCounter[R_BITS+i] = \
                unsatCounter2[N_BITS+i] + unsatCounter2[N_BITS+R_BITS+i];
    }
}


/**
 * Work in progress function that uses 1st round decode for decaps_board
 *  rc = decode_1st_round(e, syndrome.raw, h0_compact, h1_compact, u);
 * @param e OUT the shared secret recovered from the syndrome
 * @param syndrome the syndrome to decode
 * @param h0 first half of the secret key 
 * @param h1 second hald of the secret key
 * @param u  represents that the syndrome must become a zero-vector
 * @param params the parameters for the algorithm
 */
void decode(uint8_t *e, uint8_t *syndrome, uint8_t *h0,
            uint8_t *h1, uint32_t u, const bike2_params_t *params)
{
    //Use Pseudo-Random Number Generator in conjunction with the seed to get state 
    // double_seed_t seeds = {0};
    // get_seeds(&seeds, DECAPS_SEEDS);
    // aes_ctr_prf_state_t prf_state = {0};
    // init_aes_ctr_prf_state(&prf_state, (MASK(32)), &seeds.s1);

    // Compute the first column of each parity-check block:
    h0_col[PUBLIC_KEY_WEIGHT] = {0};
    h1_col[PUBLIC_KEY_WEIGHT] = {0};

    getCol(h0_col, h0);
    getCol(h1_col, h1);

    // J: list of positions involved in more than
    // (threshold - delta) unsatisfied p.c. equations:
    uint32_t J[DELTA_BIT_FLIPPING][N_BITS] = {0};
    uint32_t sizeJ[DELTA_BIT_FLIPPING] = {0};
    
    // count the number of unsatisfied parity-checks:
    uint8_t unsatParityCount[N_BITS] = {0};
    computeUnsatCounter(&unsatParityCount, syndrome, h0, h1);



    

    // LINE 1 of One-Round Bit Flipping Algorithm:
    uint32_t threshold = get_predefined_threshold_var(s);
    DMSG("\t\t\tThreshold: %d\n", threshold);

    // LINES 2-4 of One-Round Bit Flipping Algorithm:
    for (uint32_t i = 0; i < N_BITS; i++)
    {
        if (unsatParityCount[i] > threshold - DELTA_BIT_FLIPPING)
        {
            uint32_t difference = threshold - MIN(threshold, unsatParityCount[i]);
            J[difference][sizeJ[difference]] = i;
            sizeJ[difference]++;
        }
    }

    // LINES 5-6 of One-Round Bit Flipping Algorithm:
    for (uint32_t i = 0; i < sizeJ[0]; i++)
    {
        flipAdjustedErrorPosition(e, J[0][i]);
    }

    recompute_syndrome(s, sizeJ[0], J[0], h0_compact, h1_compact);

    DMSG("\t\tStep 1. Weight(syndrome): %u Weight(error): %u.\n", getHammingWeight(s, R_BITS), getHammingWeight(e, N_BITS));

    // check if decoding finished:
    if (getHammingWeight(s, R_BITS) <= u)
    {
        DMSG("\t\tWeight(syndrome): %d\n", getHammingWeight(s, R_BITS));
        return 0;
    }

    // LINES 7-10 of One-Round Bit Flipping Algorithm:
    for (uint32_t i = 0; getHammingWeight(s, R_BITS) > S_BIT_FLIPPING && i < MAX_IT_LOOP1; i++)
    {
        for (int l = 0; l < DELTA_BIT_FLIPPING; l++)
        {
            check(e, h0_compact_col, h1_compact_col, h0_compact, h1_compact, s,
                    J[l], sizeJ[l], DV/2);
        }
        DMSG("\t\tStep 2 (loop). Weight(syndrome): %u Weight(error): %u\n", getHammingWeight(s, R_BITS), getHammingWeight(e, N_BITS));
    }

    // check if decoding finished:
    if (getHammingWeight(s, R_BITS) <= u)
    {
        DMSG("\t\tWeight(syndrome): %d\n", getHammingWeight(s, R_BITS));
        return 0;
    }

    // LINES 11-12 of One-Round Bit Flipping Algorithm:
    uint32_t errorPos[R_BITS] = {0};
    int countError = 0;
    for (uint32_t i = 0; i < 2*R_BITS; i++)
    {
        if (e[i])
        {
            uint32_t posError = i;
            if (i != 0 && i != R_BITS)
            {
                // the position in e is adjusted since syndrome is transposed
                posError = (i > R_BITS)? ((N_BITS - i)+R_BITS) : (R_BITS - i);
            }
            errorPos[countError++] = posError;
        }
    }
    for (int j = 0; j < countError; j++)
    {
        uint32_t pos = errorPos[j];
        uint32_t counter_unsat_pos;

        if (pos < R_BITS)
        {
            counter_unsat_pos = ctr(h0_compact_col, pos, s);
        }
        else
        {
            counter_unsat_pos = ctr(h1_compact_col, pos-R_BITS, s);
        }

        if (counter_unsat_pos > (DV/2))
        {
            flipAdjustedErrorPosition(e, pos);
            recompute_syndrome(s, 1, &pos, h0_compact, h1_compact);
        }
    }

    DMSG("\t\tStep 3. Weight(syndrome): %u Weight(error): %u.\n", getHammingWeight(s, R_BITS), getHammingWeight(e, N_BITS));

    // check if decoding finished:
    if (getHammingWeight(s, R_BITS) <= u)
    {
        DMSG("\t\tWeight(syndrome): %d\n", getHammingWeight(s, R_BITS));
        return 0;
    }

    // LINES 13-15 of One-Round Bit Flipping Algorithm:
    for (uint32_t k = 0; getHammingWeight(s, R_BITS) > u && k < MAX_IT_LOOP2; k++)
    {
        // find a random non-zero position in the syndrome:
        uint32_t i = 0;
        get_rand_mod_len(&i, R_BITS, &prf_state);
        while (!s[i])
            i = (i + 1) % R_BITS;

        int errorFound = 0;
        for (int j = 0; j < DV && !errorFound; j++)
        {
            // finding position of 1 in the i-th row:
            uint32_t pos = (h0_compact[j] + i) % R_BITS;
            int counter_unsat_pos = ctr(h0_compact_col, pos, s);
            if (counter_unsat_pos > (DV/2))
            {
                flipAdjustedErrorPosition(e, pos);
                recompute_syndrome(s, 1, &pos, h0_compact, h1_compact);
                errorFound = 1;
                DMSG("\t\t\tFlipped position %d which has counter_unsat_pos: %d\n", pos, counter_unsat_pos);
            }
        }
        for (int j = 0; j < DV && !errorFound; j++)
        {
            // finding position of 1 in the i-th row:
            uint32_t pos = (h1_compact[j] + i) % R_BITS;
            pos += R_BITS;
            int counter_unsat_pos = ctr(h1_compact_col, pos, s);
            if (counter_unsat_pos > (DV/2))
            {
                flipAdjustedErrorPosition(e, pos);
                recompute_syndrome(s, 1, &pos, h0_compact, h1_compact);
                errorFound = 1;
                DMSG("\t\t\tFlipped position %d which has counter_unsat_pos: %d\n", pos, counter_unsat_pos);
            }
        }
        DMSG("\t\t\t\tStep 4 (loop). Weight(syndrome): %d Weight(error): %d\n", getHammingWeight(s, R_BITS), getHammingWeight(e, N_BITS));
    }

    // check if decoding succeeded:
    if (getHammingWeight(s, R_BITS) <= u)
    {
        DMSG("\t\tWeight(syndrome): %d\n", getHammingWeight(s, R_BITS));
        return 0;
    }

    return -1;
}
