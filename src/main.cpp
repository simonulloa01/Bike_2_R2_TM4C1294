

#include <libopencm3/lm4f/gpio.h>
#include <libopencm3/lm4f/systemcontrol.h>

int main(void) {
    periph_clock_enable(RCC_GPION);
    
    // Configure PN0 as output
    gpio_mode_setup(GPION_BASE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0);
    
    while (1) {
        // Toggle PN0
        gpio_toggle(GPION_BASE, GPIO0);
        
        // Delay
        for (int i = 0; i < 1000000; i++) {
            __asm__("nop");
        }
    }
    
    return 0;
}
