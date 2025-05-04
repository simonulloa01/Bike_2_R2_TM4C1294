
#include <stdio.h> // Include standard I/O for printf
#include "bike2-board.h"

bool is_zero(const uint8_t *vec, size_t size_bytes)
{
    for (size_t i = 0; i < size_bytes; i++)
    {
        if (vec[i] != 0)
        {
            return false;
        }
    }
    return true;
}

// Helper function to calculate the Hamming weight of the product H_j * s'
// This is a simplified placeholder(not preformance optimized)
// H is conceptually [h0^T | h1^T]
// j is the bit index (0 to n-1)
uint32_t calculate_unsat_count(uint32_t j, const uint8_t *h0, const uint8_t *h1,
                               const uint8_t *s_prime, const bike2_params_t *params)
{
    uint32_t r = params->block_size_bits;
    uint32_t r_bytes = params->block_size;
    uint32_t count = 0;
    const uint8_t *h_current = (j < r) ? h0 : h1; // Choose h0 or h1
    uint32_t bit_offset = (j < r) ? j : j - r;     // Offset within h0 or h1

    // Iterate through the bits of s_prime (which has length r)
    for (uint32_t i = 0; i < r; ++i)
    {
        // Check if the i-th bit of s_prime is set
        uint32_t s_byte_idx = i / 8;
        uint32_t s_bit_pos = i % 8;
        bool s_bit_is_set = (s_prime[s_byte_idx] & (1u << s_bit_pos)) != 0;

        if (s_bit_is_set)
        {
            // Find the corresponding bit in the j-th column of H
            // This corresponds to the i-th bit of the cyclic shift of h_current by bit_offset
            uint32_t h_bit_idx = (i - bit_offset + r) % r; // Calculate the index in the original h polynomial
            uint32_t h_byte_idx = h_bit_idx / 8;
            uint32_t h_bit_pos = h_bit_idx % 8;

            if (h_byte_idx < r_bytes && (h_current[h_byte_idx] & (1u << h_bit_pos)) != 0)
            {
                count++;
            }
        }
    }
    return count;
}

// Using a simple fixed threshold for demonstration: w/4
uint32_t calculate_threshold(const uint8_t *s_prime, const bike2_params_t *params)
{
    //Placeholder read Section 2.4.2.
    return (params->row_weight / 4); // Placeholder: Use 142/4 = 35.
}




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
 * @param sk the secret key (h0, h1) - size 2 * r_bytes
 * @param ct the ciphertext (c) - size r_bytes
 * @param ss OUT the encapsulation key (K) - size code_length/8
 * @param params the parameters for the algorithm
 */
void decap_board(const uint8_t *sk, const uint8_t *ct, uint8_t *ss, const bike2_params_t *params)
{
    uint32_t r = params->block_size_bits;
    uint32_t n = r * 2; // n = 2r
    uint32_t r_bytes = params->block_size;
    uint32_t n_bytes = r_bytes * 2;

    uint8_t h0[r_bytes];
    uint8_t h1[r_bytes];
    uint8_t syndrome[r_bytes];         // s = c*h0
    uint8_t e_prime[n_bytes];          // Estimated error (e0', e1')
    uint8_t s_diff[r_bytes];           // Current syndrome difference: s - (e0'*h0 + e1'*h1)
    uint8_t e0_h0[r_bytes];            // 
    uint8_t e1_h1[r_bytes];            // 
    uint8_t current_syndrome[r_bytes]; // e0'*h0 + e1'*h1
    uint8_t flip_vector[n_bytes];      // Bits to flip in current iteration

    // Ensure buffers are zeroed
    memset(h0, 0, r_bytes);
    memset(h1, 0, r_bytes);
    memset(syndrome, 0, r_bytes);
    memset(e_prime, 0, n_bytes);
    memset(s_diff, 0, r_bytes);
    memset(e0_h0, 0, r_bytes);
    memset(e1_h1, 0, r_bytes);
    memset(current_syndrome, 0, r_bytes);
    memset(flip_vector, 0, n_bytes);
    // Assuming ss is sized appropriately for the hash output (e.g., 32 bytes)
    // memset(ss, 0, HASH_OUTPUT_SIZE_BYTES);

    // Extract h0 and h1 from the secret key sk
    memcpy(h0, sk, r_bytes);
    memcpy(h1, sk + r_bytes, r_bytes);

    // Compute the syndrome: s = ct * h0 (mod X^r - 1)
    printf("Calculating syndrome s = ct * h0...\n");
    modMult(syndrome, ct, h0, r_bytes, params);

    // --- Begin Bit Flipping Decode ---
    printf("Starting decoder...\n");
    memcpy(s_diff, syndrome, r_bytes); // Initialize s_diff = s
    // e_prime is already initialized to zero

    uint32_t iterations = 0;
    while (!is_zero(s_diff, r_bytes) && iterations < MAX_DECODER_ITERATIONS)
    {
        iterations++;
        printf("Decoder Iteration: %u\n", iterations);
        memset(flip_vector, 0, n_bytes); // Reset flip vector for this iteration

        // Calculate threshold T for this iteration
        uint32_t threshold = calculate_threshold(s_diff, params);
        printf("  Threshold T = %u\n", threshold);

        // Iterate through all possible error positions j = 0 to n-1
        uint32_t flipped_count_iter = 0;
        for (uint32_t j = 0; j < n; ++j)
        {
            // Calculate unsatisfied count: |h_j * s_diff|
            uint32_t unsat_count = calculate_unsat_count(j, h0, h1, s_diff, params);

            // If unsat_count >= T, mark bit j for flipping
            if (unsat_count >= threshold)
            {
                uint32_t flip_byte_idx = j / 8;
                uint32_t flip_bit_pos = j % 8;
                flip_vector[flip_byte_idx] |= (1u << flip_bit_pos);
                flipped_count_iter++;
            }
        }
        printf("  Bits marked for flipping in iteration %u: %u\n", iterations, flipped_count_iter);

        // Update the estimated error vector e' by flipping bits (e' = e' XOR flip_vector)
        modAdd(e_prime, e_prime, flip_vector, n_bytes);

        // Recalculate the syndrome difference: s_diff = s - (e0'*h0 + e1'*h1)
        // current_syndrome = e0'*h0 + e1'*h1
        modMult(e0_h0, e_prime, h0, r_bytes, params);           // e0'*h0
        modMult(e1_h1, e_prime + r_bytes, h1, r_bytes, params); // e1'*h1
        modAdd(current_syndrome, e0_h0, e1_h1, r_bytes);        // e0'*h0 + e1'*h1
        modAdd(s_diff, syndrome, current_syndrome, r_bytes);    // s - current_syndrome (XOR is add/sub)

        // Optional: Print weight of s_diff for debugging
        // uint32_t s_diff_weight = get_hamming_weight(s_diff, r);
        // printf("  Weight of s_diff after iteration %u: %u\n", iterations, s_diff_weight);

    } // End of decoder loop

    printf("Decoder finished after %u iterations.\n", iterations);

    // --- End Bit Flipping Decoder ---

    // 3. Verification and Hash
    uint32_t final_weight = get_hamming_weight(e_prime, n);
    printf("Final error weight |e'| = %u (Target T = %d)\n", final_weight, params->target_error);

    // Check if decoding was successful (syndrome is zero AND weight is correct)
    if (is_zero(s_diff, r_bytes) && final_weight == params->target_error)
    {
        printf("Decoding successful!\n");
        // Compute K = K(e0', e1')
        printf("Hashing recovered error vector (e0', e1')...\n");
        
        hash(ss, e_prime, n_bytes); // ss is output, e_prime is input, length is input length
    }
    else
    {
        printf("Decoding failed!\n");
        // Handle failure: output a default value or signal an error.
        printf("Hashing failed error vector as fallback...\n");
        
        hash(ss, e_prime, n_bytes); // Hash whatever e_prime is
        // Or hash a fixed value:
        // uint8_t dummy[n_bytes];
        // memset(dummy, 0, n_bytes);
        // hash(ss, dummy, n_bytes);
    }
    printf("Decap complete.\n");
}


void hash(uint8_t *output, const uint8_t *input, const uint32_t length) // length is input length in bytes
{
    // Assumes fixed output size of 32 bytes (AES256_KEY_SIZE)
    const size_t output_len_bytes = AES256_KEY_SIZE; // Fixed output size assumption

    printf("WARNING: Using placeholder HASH function (fixed %zu byte output)!\n", output_len_bytes);

    memset(output, 0, output_len_bytes); // Zero out the buffer first

    // Simple pseudo-hash: XOR bytes of input into output buffer repeatedly
    if (output_len_bytes > 0)
    { // Avoid division by zero
        for (uint32_t i = 0; i < length; ++i)
        {
            output[i % output_len_bytes] ^= input[i];
        }
    }

    // Ensure output has some value even if input is short/zero
    if (output_len_bytes > 0)
    {
        output[0] ^= 0xAA;
        if (output_len_bytes > 1)
        {
            output[output_len_bytes - 1] ^= 0x55;
        }
    }
}
