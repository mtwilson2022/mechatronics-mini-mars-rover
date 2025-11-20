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
    
    robotTaskState = LINE_FOLLOW;
    
    canyonSensorState = STRAIGHT;
    lineSensorState = GO_CENTER;
    
    while (1) {
//        switch (robotTaskState) {
//            case LINE_FOLLOW:
//                lineNav();
//                
//                if (lineSensorState == NO_ACTIVE) {
//                    robotTaskState = CANYON_NAV;
//                }
//                
//                break;
//                
//            case CANYON_NAV:
//                canyonNav();
//                
//                if (senseLineEndOfCanyon()) {
//                    // put something here to get the robot moving in the right direction
//                    robotTaskState = LINE_FOLLOW;
//                }
//                
//                break;
//        }
        
        switch (robotTaskState) {
            case LINE_FOLLOW:
                lineNav();
                
                _LATB7 = 0;
                _LATB8 = 1;
                _LATB9 = 0;
                
                if ((RIGHT_SONAR_SIG < SONAR_THRESHOLD) && (CENTER_LINE_SIG < LINE_SENSOR_THRESHOLD)) {
                    robotTaskState = SAMPLE_RETURN;
                }
                
                break;
                
            case SAMPLE_RETURN:
                OC2R = 0;
                OC3R = 0;
                delay(50000);
                
                if (senseBallWhite()) {
                    depositWhiteBall();
                }
                else {
                    depositBlackBall();
                }
                
                delay(20000);
                
                robotTaskState = LINE_FOLLOW;
                
                break;
        }
    }
    
    return 0;
}
