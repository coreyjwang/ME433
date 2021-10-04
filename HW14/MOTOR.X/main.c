#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include <proc/p32mx170f256b.h>
#include<math.h>

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

void delay() { // Delay function
    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT() <= 12000000) { ; }
}


void apply_duty_cycle(int degrees) {
    // Check that inputs are valid:
    if (degrees >= 0 & degrees <= 180) {
    //0.5 ms (duty cycle = .025 -> OC5RS=1562.5) - 0 degrees
    //2.5 ms (duty cycle = .125 -> OC5RS=7812.5) - 180 degrees
    // Function to relate two: oc5rs = 34.61 * degrees + 1570
//        OC5RS = (int) 34.61 * degrees + 1570;
        OC5RS = (int) 75 * degrees + 1570; // These values work better
    }
}


int main(void) {
    // call initializations
    __builtin_disable_interrupts(); // disable interrupts while initializing things
    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;
    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;
    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    // do your TRIS and LAT commands here
    TRISBbits.TRISB4 = 1; // Input pin
    TRISAbits.TRISA4 = 0; // Output pin
    LATAbits.LATA4 = 0; // Initially on
    // Select pin for OC5
    RPB2R = 0b0110; // Set pin B2 to OC5
    __builtin_enable_interrupts();
  
    // set the pin you want to use to OC5
    T2CONbits.TCKPS = 11;     // 11 = 1:256 prescale value
    PR2 = 62499; // period = (PR2+1) * N * 12.5 ns = 0.02 s, 50 Hz=> (PR2+1) * N = 16,000,000 => N=256, PR2 = 62499
    TMR2 = 0;                // initial TMR2 count is 0
    OC5CONbits.OCM = 0b110;  // PWM mode without fault pin; other OCxCON bits are defaults
    OC5RS = 0;             // duty cycle = OCxRS/(PR2+1) default to 0
    OC5R = 0;              // initialize before turning OCx on; afterward it is read-only
    T2CONbits.ON = 1;        // turn on Timer2
    OC5CONbits.ON = 1;       // turn on OCx
  
    // the rest of your code
    // set OCxRS to get between a 0.5ms and 2.5ms pulse out of the possible 20ms (50Hz)
//    apply_duty_cycle(10);
//    delay();
//    apply_duty_cycle(170);
//    delay();

    // Move servo sinusoidally
    while (1) {
        float time = 0;
        time += 0.02;
        int degrees = (int) abs(180*sin(6.28 * time));
        apply_duty_cycle(degrees);
        delay();
    }

}