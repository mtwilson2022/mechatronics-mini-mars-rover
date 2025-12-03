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
    _RCDIV = 0b010; // oscillator: divide by 4 postscaling (so Fosc = 2 MHz)
    
    configurePins();
    setupMotors();
    configAD();
    
    STRAIGHT_LED = 0;
    LEFT_LED = 0;
    RIGHT_LED = 0;
    
//    static RobotTaskState robotTaskState = LINE_FOLLOW;
//    
//    CanyonSensorState canyonSensorState = RIGHT;
//    static LineSensorState lineSensorState = GO_CENTER;
//    
    while (1) {
        switch (robotTaskState) {
            case LINE_FOLLOW:
                
                lineNav();
                
                if (lineSensorState == NO_ACTIVE) {
                    robotTaskState = CANYON_NAV;
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
                
                break;
                
                
            case CANYON_NAV:
                
//                STRAIGHT_LED = 0;
//                LEFT_LED = 1;
//                RIGHT_LED = 1;
                
                canyonNav(canyonSensorState);
                
                if (senseLineEndOfCanyon()) {
                    // *TODO* put something here to get the robot moving in the right direction
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
                
                delay(10000); // instead: turn right/left until on line
                
                // is the robot reliably on the line at this point?
                robotTaskState = LINE_FOLLOW;
                
                break;
                
            case DATA_TRANSMIT:
                // move into lander (this could also be state transition logic)
                
                // move the pointer up slowly until it sees IR emitter.
                // then turn on the laser. The course is complete.
                pointLaser();
                break;
        }

    }
    
    return 0;
}
