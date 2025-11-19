/* 
 * File:   Setup.c
 * Authors: Matthew Wilson, Sean Reisinger, William Bowler, Caden Meyer
 * Created for an ME EN 330 "Mars Rover" project
 * 
 * Setup functions for I/O, PWM, ADC.
 *
 * Created on November 18, 2025, 6:12 PM
 */

#include <xc.h>
#include "Configurations.h"
#include "Setup.h"


void configurePins() {
    ANSB = 0;           //initially set all analog select pins to digital
    ANSA = 0;

    _ANSB12 = 1;    //line left sensor
    _ANSB13 = 1;    //line center sensor
    _ANSB14 = 1;    //line right sensor
    _ANSB2 = 1;     //sonar front sensor
//    _ANSA2 = 1;   //second sonar sensor, not currently used
    _ANSA3 = 1;     //IR right sensor
    
    
    
    _TRISA0 = 0;    //motor one (left) dir
    _TRISA1 = 0;    //motor two (right) dir
    _TRISB7 = 0;    //left led
    _TRISB8 = 0;    //center led
    _TRISB9 = 0;    //right led
    _TRISA3 = 1;    //IR right sensor
    _TRISB12 = 1;   //line left sensor
    _TRISB13 = 1;   //line center sensor
    _TRISB14 = 1;   //line right sensor

}


void configAD() {
    _ADON = 0;          // AD1CON1<15> -- Turn off A/D during config
    // Clear all A/D registers
    AD1CON1 = 0; 
    AD1CON2 = 0; 
    AD1CON3 = 0; 
    AD1CON5 = 0; 
    AD1CSSL = 0; 
    AD1CSSH = 0;
    
    // AD1CON1 register
    _ADSIDL = 0;    // AD1CON1<13> -- A/D continues while in idle mode
    _MODE12 = 1;    // AD1CON1<10> -- 12-bit A/D operation
    _FORM = 0;      // AD1CON1<9:8> -- Unsigned integer output
    _SSRC = 7;      // AD1CON1<7:4> -- Auto conversion (internal counter)
    _ASAM = 1;      // AD1CON1<2> -- Auto sampling
    
    // AD1CON2 register
    _PVCFG = 0;     // AD1CON2<15:14> -- Use VDD as positive ref voltage
    _NVCFG = 0;     // AD1CON2<13> -- Use VSS as negative ref voltage
    _BUFREGEN = 1;  // AD1CON2<11> -- Result appears in buffer
                    // location corresponding to channel, e.g., AN12
                    // results appear in ADC1BUF12
    _CSCNA = 1;     // AD1CON2<10> -- Scans inputs specified in AD1CSSx
                    // registers
    _SMPI = 2;    // AD1CON2<6:2> -- Results sent to buffer after n conversion
                    // For example, if you are sampling 4 channels, you
                    // should have _SMPI = 3;
    _ALTS = 0;      // AD1CON2<0> -- Sample MUXA only
    
    // AD1CON3 register -- Change _SAMC and _ADCS based on your
    // selection of oscillator and postscaling
    _ADRC = 0;      // AD1CON3<15> -- Use system clock
    _SAMC = 4;      // AD1CON3<12:8> -- Auto sample every A/D period TAD
    _ADCS = 0;      // AD1CON3<7:0> -- A/D period TAD = TCY
    
    // AD1CHS register
    _CH0NA = 0;     // AD1CHS<7:5> -- Measure voltages relative to VSS
    
    // AD1CSSL register
    // SET THE BITS CORRESPONDING TO CHANNELS THAT YOU WANT TO SAMPLE
    AD1CSSL = 0b0001110000000000;
    
    _ADON = 1;      // AD1CON1<15> -- Turn on A/D
}


void setupMotors() {
    // configure motor 1 (left motor; Pin 4)
    OC2CON1 = 0;
    OC2CON2 = 0;
    OC2CON1bits.OCTSEL = 0b111;     //Timer select bits. 0b111 is sys clock.
    OC2CON1bits.OCM = 0b110;        //0b110 = Edge aligned pwm.
    OC2CON2bits.SYNCSEL = 0x1F;     //0b00011111 = this OC module
    OC2CON2bits.OCTRIG = 0;         //0 = Synchronizes OCx with source designated by the SYNCSELx bits
    DIRECTION_MOTOR_ONE = 0;        //Initial Direction forward
    
    // configure motor 2 (right motor; Pin 5)
    OC3CON1 = 0;
    OC3CON2 = 0;
    OC3CON1bits.OCTSEL = 0b111;     //Timer select bits. 0b111 is sys clock.
    OC3CON1bits.OCM = 0b110;        //0b110 = Edge aligned pwm.
    OC3CON2bits.SYNCSEL = 0x1F;     //0b00011111 = this OC module
    OC3CON2bits.OCTRIG = 0;         //0 = Synchronizes OCx with source designated by the SYNCSELx bits
    DIRECTION_MOTOR_TWO = 1;        //Initial Direction forward
    
    // set period for both motors; start duty cycle at 0
    OC2RS = PERIOD; // @ F_cy = 250kHz, 1 rev per sec in 1/4 step mode is 312
    OC2R = 0;
    OC3RS = PERIOD;
    OC3R = 0;
}
