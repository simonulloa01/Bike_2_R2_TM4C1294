
#include "bike2-board.h"


void keygen_board(uint8_t *pk, uint8_t *sk, const bike2_params_t *params){
    //generate 2 sparse polynomials h0 and h1
    uint8_t h0[params->block_size];
    uint8_t h1[params->block_size];
    generate_sparse_polynomial(h0, params->block_size, params->row_weight/2, params->code_length);
    generate_sparse_polynomial(h1, params->block_size, params->row_weight/2, params->code_length);

    //invert h0 to get h0_inv

    // h = h1 * h0_inv

    //assembly the keys pk = h and sk = (h0, h1)

    //copy sk
    memcpy(sk, h0, params->block_size);
    memcpy(sk + params->block_size, h1, params->block_size);

}
void encrypt_board(const uint8_t *pk, uint8_t *ct, uint8_t *ss, const bike2_params_t *params){
    
}

void decrypt_board(const uint8_t *sk, const uint8_t *ct, uint8_t *ss, const bike2_params_t *params){

}