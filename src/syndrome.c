#include "syndrome.h"
#include "defs.h"

void compute_syndrome(uint8_t *syndrome, uint8_t *out_syndrome,
                      const uint8_t *e0,
                      const uint8_t *e1,
                      const uint8_t *h0,
                      const uint8_t *h1,
                      const uint8_t *original_s,
                      const bike2_params_t *params)
{
    
    uint8_t temp0[params->block_size];
    uint8_t temp1[params->block_size];
    

}