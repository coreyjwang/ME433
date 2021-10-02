#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include <proc/p32mx170f256b.h>
#include<math.h>
#include "ST7789.h"
#include "spi.h"
#include "font.h"

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


char m[15]; // message

void progress_bar(unsigned short x, unsigned short y, unsigned short length) {
    int i, j;
    for (i=0; i<length; i+=1) {
        for (j=0; j<10; j+=1) {
            LCD_drawPixel(x+i, y+j, RED);
        }
    }
}

int main() {
    __builtin_disable_interrupts(); // disable interrupts while initializing things
    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    __builtin_enable_interrupts();
    
    initSPI();
    LCD_init();
    LCD_clearScreen(BLACK);
    
    int i;
    for (i=0; i<=100; i+=1) {
        _CP0_SET_COUNT(0);
        LCD_clearScreen(BLACK);
        sprintf(m, "Hello world %d!", i);
        drawString(28,32,WHITE,m);
        progress_bar(28, 100, i);
        
        // FPS count
        char fps_count[10];
        sprintf(fps_count, "%d FPS", (int) (_CP0_GET_COUNT() / 480000));
        drawString(180, 32,WHITE,fps_count);
    }
    
    // Since characters are 5 x 8, the screen can fit 48 x 30 = 1440 total
    // Fill screen with fps counter
    while(1) {
        _CP0_SET_COUNT(0);
        LCD_clearScreen(RED);
        // FPS count
        char fps_count[10];
        sprintf(fps_count, "%d FPS", (int) (_CP0_GET_COUNT() / 480000));
        drawString(180, 32,WHITE,fps_count);
    }
    
}
