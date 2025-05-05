#include <stdio.h>
#include <time.h>
#include "bike2-board.h"
#include "modArith.h"

const uint32_t SIZE = 32;

int main(void) {

    // Set up arguments for polyMult function.
    uint8_t dst[SIZE];
    uint8_t a[SIZE];  
    uint8_t b[SIZE];

    memset(dst, 0, sizeof(dst));
    memset(a, 0, sizeof(a)); 
    memset(b, 0, sizeof(b));

    clock_t start = clock();
    
    // test modMult latency
    modMult(dst, a, b, SIZE, &bike2_params_level_1);

    clock_t end = clock();

    // Print out execution time in CPU cycles. 
    printf("Test ModMult Latency: %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

    return 0;
}
