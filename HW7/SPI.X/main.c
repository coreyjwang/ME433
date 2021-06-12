#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include "spi.h"
#include<math.h>
#include <proc/p32mx170f256b.h>

void spi_dac(unsigned char c, unsigned short v) {
    unsigned short p;
    p = c << 15;
    p |= 0b111<<12;
    p |= v << 2;
    LATAbits.LATA0 = 0; // Set CS low
    spi_io(p>>8);
    spi_io(p);
    LATAbits.LATA0 = 1;
}


int main() {
    // Initialize SPI
    __builtin_disable_interrupts();
    initSPI();
    __builtin_enable_interrupts();
    // Timer variable
    double t = 0;
    // Loop
    while(1) {
        spi_dac(0, 200 * sin(2 * 3.1415 * 2 * t) + 200); // 2 Hz sine wave
        spi_dac(1, 127.32395 * asin(sin(3.1415 * 2 * t)) + 200);
        // Increment time
        t = t + 0.05;
        // Delay
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT() < 48000 / 2) { // 100 Hz
        }
        _CP0_SET_COUNT(0);
    }
}