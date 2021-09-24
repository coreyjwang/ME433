#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include <proc/p32mx170f256b.h>
#include "i2c_master_noint.h"


// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
#pragma config FNOSC = FRCPLL // use internal oscillator with pll
#pragma config FSOSCEN = OFF // disable secondary oscillator
#pragma config IESO = OFF // disable switching clocks
#pragma config POSCMOD = OFF // Internal RC
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // disable clock switch and FSCM
#pragma config WDTPS = PS1048576 // use largest wdt
#pragma config WINDIS = OFF // use non-window mode wdt
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations


// Write address: 0100 000 0
// Read  address: 0100 000 1


void delay() { // Delay function
    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT() <= 12000000) { ; }
}


void setPin(unsigned char address, unsigned char register_1, unsigned char value) {
    i2c_master_start();
    i2c_master_send(address); // Address
    i2c_master_send(register_1); // Register
    i2c_master_send(value); // Value
    i2c_master_stop();
}


unsigned char readPin(unsigned char address, unsigned char register_1) {
    i2c_master_start();
    i2c_master_send(0b01000000); // Address-write
    i2c_master_send(register_1); // Register to read from
    i2c_master_restart();
    i2c_master_send(0b01000001); // Address-read
    unsigned char result = i2c_master_recv(); // Get receive value
    i2c_master_ack(1); // Acknowledge received
    i2c_master_stop();
    return result;
}


int main() {
    // Initial setup
    i2c_master_setup();
    // Set IODIRA (Register 0x00) to 0x00 to set A pins as output
    setPin(0b01000000, 0, 0);
    // Set IODIRB (Register 0x01) to 0xFF to set B pins as input
    setPin(0b01000000, 1, 255);

    // Set up pin A4 of PIC for blinking LED
    TRISAbits.TRISA4 = 0; // Output pin
    LATAbits.LATA4 = 0; // Initially off

//    // Chip initialization and blink the LED on GPA7 to test (comment out later)
//    while (1) {
//        delay();
//        setPin(0b01000000, 0x14, 0x0); // Set OLATA to 0
//        delay();
//        setPin(0b01000000, 0x14, 0xFF); // Set OLATA to 1
//    }

    // Read from GBB0 and control GPA7
    while (1) {
        // Blink LED connected to PIC
        LATAbits.LATA4 = !LATAbits.LATA4; // Invert output of A each loop
        delay();

        // Read GPB0
        if (readPin(0b01000001, 0x13) == 0) { // If button is pushed GPIOB = 0x13 goes low
            setPin(0b01000000, 0x14, 0xFF); // Set OLATA to 1 (Turn LED on)
        }
        else {
            setPin(0b01000000, 0x14, 0x0); // Set OLATA to 0 (Turn LED off)
        }
    }
}
