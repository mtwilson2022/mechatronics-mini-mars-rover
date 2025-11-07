/* 
 * File:   main.c
 * Author: mtwil
 *
 * Created on November 6, 2025, 12:27 PM
 */

#include <stdio.h>
#include <stdlib.h>

#include "Configurations.h"

#pragma config FNOSC = FRCDIV   // 8 MHz oscillator with postscaling
#pragma config OSCIOFNC = OFF   // Turn off clock output on pin 8
#pragma config SOSCSRC = DIG    // Turn off secondary oscillator on pins 9&10


// Global variables
int steps_one = 0;                  //step counter for stepper MOTOR_ONE
int steps_two = 0;                  //step counter for stepper MOTOR_TWO


//Configuration and Setup
enum {CENTER, LEFT_CENTER, RIGHT_CENTER, NO_ACTIVE} lineSensorState;

//Interrupts
void __attribute__((interrupt, no_auto_psv)) _OC2Interrupt(void)
{
    // OC2 Interrupt Service Routine MOTOR_ONE
    _OC2IF = 0;
    steps_one += 1;
}
void __attribute__((interrupt, no_auto_psv)) _OC3Interrupt(void)
{
    // OC3 Interrupt Service Routine MOTOR_TWO
    _OC3IF = 0;
    steps_two += 1;
}

//Extra functions outside the main loop
int senseLeft() {
    if (_RB12) {
        return 1;
    }
    return 0;
}
int senseRight() {
    if (_RB14) {
        return 1;
    }
    return 0;
}
int senseCenter() {
    if (_RB13) {
        return 1;
    }
    return 0;
}
int senseNone() {
    if (_RB14 || _RB13 ||_RB12) {
        return 0;
    }
    return 1;
}

/*
 * 
 */
int main(int argc, char** argv) {

    _RCDIV = 100; // oscillator: divide by 16 postscaling (so Fosc = 500 kHz)
    configurePins();
    setupMotors();
    lineSensorState = NO_ACTIVE;

    
    while (1) {

        switch (lineSensorState) {
            case CENTER:
                // state transition?
                if (senseLeft()) {
                    lineSensorState = LEFT_CENTER;
                }
                else if (senseRight()) {
                    lineSensorState = RIGHT_CENTER;
                }
                else if (senseNone()) {
                    lineSensorState = NO_ACTIVE;
                }
                
                // Go straight
                OC2R = DUTY;
                OC3R = DUTY;
                
                break;
            
            case LEFT_CENTER:
                // state transition?
                if (senseCenter()) {
                    lineSensorState = CENTER;
                }
                else if (senseRight()) {
                    lineSensorState = RIGHT_CENTER;
                }
                else if (senseNone()) {
                    lineSensorState = NO_ACTIVE;
                }
                
                // Go left (left motor off)
                OC2R = 0;
                OC3R = DUTY;
                break;
                
            case RIGHT_CENTER:
                DEBUG_LED = 1;
                // state transition?
                if (senseLeft()) {
                    lineSensorState = LEFT_CENTER;
                }
                else if (senseCenter()) {
                    lineSensorState = CENTER;
                }
                else if (senseNone()) {
                    lineSensorState = NO_ACTIVE;
                }
                
                // go right (right motor off)
                OC2R = DUTY;
                OC3R = 0;
               
                break;
                
            case NO_ACTIVE:
                // state transition?
                if (senseLeft()) {
                    lineSensorState = LEFT_CENTER;
                }
                else if (senseRight()) {
                    lineSensorState = RIGHT_CENTER;
                }
                else if (_RB13) {
                    lineSensorState = CENTER;
                }
                
                // don't move
                OC2R = 0;
                OC3R = 0;
                break;
                
//            default:
//                lineSensorState = NO_ACTIVE;
        }
        

    }
    
    return (EXIT_SUCCESS);
}


void delay(int s) {
    int k = 0;
    while(k < s)
    k++;
}
