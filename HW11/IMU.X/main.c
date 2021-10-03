#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include <proc/p32mx170f256b.h>
#include<math.h>
#include "ST7789.h"
#include "spi.h"
#include "font.h"
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

// Write & read addresses for IMU
// 7-bit address: 0b1101010, last digit 1 (read) or 0 (write)

// Variables
char m[15]; // message
unsigned char data_char[14];
signed short data_short[7];

void delay() { // Delay function
    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT() <= 12000000) { ; }
}

void delay_short() { // Delay function
    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT() <= 3000000) { ; }
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
    i2c_master_send(0b11010100); // Address-write
    i2c_master_send(register_1); // Register to read from
    i2c_master_restart();
    i2c_master_send(0b11010101); // Address-read
    unsigned char result = i2c_master_recv(); // Get receive value
    i2c_master_ack(1); // Acknowledge received
    i2c_master_stop();
    return result;
}

void I2C_read_multiple(unsigned char address, unsigned char register_1, unsigned char* data, int length) {
    i2c_master_start();
    i2c_master_send(0b11010100); // Address-write
    i2c_master_send(register_1); // Register to read from
    i2c_master_restart();
    i2c_master_send(0b11010101); // Address-read
    int i;
    for (i=0; i<length-1; i+=1) { // Loop for every element except last
        data[i] = i2c_master_recv(); // Get receive value
        i2c_master_ack(0); // Continue reading
    }
    // Read last element
    data[length-1] = i2c_master_recv();
    i2c_master_ack(1); // Acknowledge received
    i2c_master_stop();
    // No return because result is stored in data
}


void inclinometer(short x_accel, short y_accel) {
    short x_accel_good, y_accel_good;
    LCD_clearScreen(BLACK);
    LCD_drawPixel(119, 119, WHITE);
    // Make sure values fit on screen
    if (x_accel < -119) {x_accel_good = -119;}
    if (x_accel > 119) {x_accel_good = 119;}
    if (y_accel < -119) {y_accel_good = -119;}
    if (y_accel > 119) {y_accel_good = 119;}
    
    int i, j;
    
    // Draw acceleration in x-direction
    if (x_accel_good >= 0) {
        for (i=0; i<x_accel_good; i+=1) {
            LCD_drawPixel(119+i, 119, WHITE);
        }
    }
    else {
        for (i=0; i>x_accel_good; i-=1) {
            LCD_drawPixel(119+i, 119, WHITE);
        }
    }
    
    // Draw acceleration in y-direction
    if (y_accel_good >= 0) {
        for (j=0; j<y_accel_good; j+=1) {
            LCD_drawPixel(119, 119+j, WHITE);
        }
    }
    else {
        for (j=0; j>y_accel_good; j-=1) {
            LCD_drawPixel(119, 119+j, WHITE);
        }
    }
}

int main() {
    // Initial I2C setup
    i2c_master_setup();
    // Initialize SPI/LCD
    initSPI();
    LCD_init();
    LCD_clearScreen(BLACK);
    
    // Set up pin A4 of PIC for blinking LED
    TRISAbits.TRISA4 = 0; // Output pin
    LATAbits.LATA4 = 1; // Initially off
    
    // Check WHO_AM_I register to confirm IMU is working, if not infinite loop
    if (readPin(0b11010101, 0x0F) != 105) {
        // Turn display red and blink LED rapidly if IMU not working
//        LCD_clearScreen(RED);
        while (1) {
            LATAbits.LATA4 = !LATAbits.LATA4; // Invert output of A each loop
            delay_short();
        }
    }
    else{
        // Turn display green and continue if IMU is working properly
//        LCD_clearScreen(GREEN);
    }
    
    // Write to several registers to initialize the chip
    setPin(0b11010100, 0x10, 0b10000010); // Accelerometer
    setPin(0b11010100, 0x11, 0b10001000); // Gyroscope (angular velocity)
    setPin(0b11010100, 0x12, 0b00000100); // Temperature (all default values)
    
    LCD_clearScreen(BLACK);
    
    while (1) {
        // Heartbeat LED
        LATAbits.LATA4 = !LATAbits.LATA4; // Invert output of A each loop
        delay();
        
        // Read multiple and reconstruct the signed short value from unsigned char array
        I2C_read_multiple(0b00000100, 0x20, &data_char, 14); // 0x20 for OUT_TEMP_L
        int i;
        for (i=0; i<13; i+=2) {
            data_short[i/2] = data_char[i] | (data_char[i+1] << 8);
        }
        
//        // Print signed short values to display
//        LCD_clearScreen(BLACK);
//        for (i=0; i<7; i+=1) {
//            sprintf(m,"%d", (int) data_short[i]);
//            drawString(28, 32+20*i, RED, m);
//        }
        
        // Inclinometer function
        inclinometer((data_short[5])/20, data_short[4]/20);
    }
}
