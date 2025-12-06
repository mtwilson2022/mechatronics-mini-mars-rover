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


RobotTaskState robotTaskState = LINE_FOLLOW;
CanyonSensorState canyonSensorState = STRAIGHT;
LineSensorState lineSensorState = CENTER;


int main(int argc, char** argv) {
    _RCDIV = 0b100; // oscillator: divide by 16 postscaling (so Fosc = 500 kHz)
    
    configurePins();
    setupMotors();
    configAD();
    
    

    
    while (1) {
        switch (robotTaskState) {
            case LINE_FOLLOW:
                
                lineNav();
                
                if (lineSensorState == NO_ACTIVE) {
                    if (checkOffLine()){
                        robotTaskState = CANYON_NAV;
                    }
                }

                
//                if (SAMPLE_IR_SIG > IR_SIG_THRESH) {
//                    robotTaskState = SAMPLE_COLLECT;
//                    goStraight(FULL_SPEED);
//                } 
                
//                if ((RIGHT_SONAR_SIG < SONAR_THRESHOLD) && (CENTER_LINE_SIG < LINE_SENSOR_THRESHOLD)) {
//                    robotTaskState = SAMPLE_RETURN;
//                    goStraight(FULL_SPEED);
//                }
                
                // TODO: add transition to DATA_TRANSMIT
//                if ((FAR_LEFT_LINE_SIG < LINE_SENSOR_THRESHOLD) && (CENTER_LINE_SIG < LINE_SENSOR_THRESHOLD)
//                        && (LEFT_LINE_SIG < LINE_SENSOR_THRESHOLD) && (RIGHT_LINE_SIG < LINE_SENSOR_THRESHOLD)) {
//                    if (sampleReturned){
//                        robotTaskState = DATA_TRANSMIT;
//                    }
//                    else {
//                        turnLeftGetOnLine();
//                    }
//                }
                
                break;
                
                
            case CANYON_NAV:

                
                canyonNav(canyonSensorState);
                
                if (senseLineEndOfTask()) {
                    turnRightGetOnLine();
                    robotTaskState = LINE_FOLLOW;
                }
                
                break;
                
                
            case SAMPLE_COLLECT:
                stopMotors();
                delay(10000);
                collectSample();
                
                // have the robot back up and get on the line, then:
                robotTaskState = LINE_FOLLOW;
                
                break;
                
                
            case SAMPLE_RETURN:
                stopMotors();
                delay(10000);
                if (senseBallWhite()) {
                    depositWhiteBall();
                }
                else {
                    depositBlackBall();
                }
                
                delay(5000); // instead: turn right/left until on line
                
                // is the robot reliably on the line at this point?
                robotTaskState = LINE_FOLLOW;
                
                break;
                
                
            case DATA_TRANSMIT:
                stopMotors();
                OC2RS = PERIOD * 4;
                OC3RS = PERIOD * 4;
                turnLeftGetOnLine();
                while (!(Collision())){
                    lineNav();
                }
                pointLaser();
                break;
        }

    }
    
    return 0;
}