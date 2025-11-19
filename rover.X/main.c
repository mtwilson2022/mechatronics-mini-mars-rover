/* 
 * File:   main.c
 * Author: mtwil
 *
 * Created on November 6, 2025, 12:27 PM
 */

#include <stdio.h>
#include <stdlib.h>

#include "Configurations.h"
#include "Setup.h"
#include "Actions.h"

#pragma config FNOSC = FRCDIV   // 8 MHz oscillator with postscaling
#pragma config OSCIOFNC = OFF   // Turn off clock output on pin 8
#pragma config SOSCSRC = DIG    // Turn off secondary oscillator on pins 9&10



int main(int argc, char** argv) {
    _RCDIV = 0b100; // oscillator: divide by 16 postscaling (so Fosc = 500 kHz)
    
    configurePins();
    setupMotors();
    configAD();
    
    STRAIGHT_LED = 0;
    LEFT_LED = 0;
    RIGHT_LED = 0;
    
    canyonSensorState = STRAIGHT;
    lineSensorState = NO_ACTIVE;
    
    while (1) {
//        canyonNav();
        lineNav();
    }
    
    return 0;
}
