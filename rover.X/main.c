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
int sampleReturned = 0;
int sampleCollected = 0;

int main(int argc, char** argv) {
    _RCDIV = 0b100; // oscillator: divide by 16 postscaling (so Fosc = 500 kHz)
    
    configurePins();
    setupMotors();
    configAD();
    

    
//////////////start code (hardcoded for start)/////////
    startMission();
///////////////////////////////////////////////////////
    
    while (1) {
        switch (robotTaskState) {
            case LINE_FOLLOW:
                
                lineNav();      
                
                if (lineSensorState == NO_ACTIVE) {
                    if (checkOffLine()){
                        delay(5000);
                        robotTaskState = CANYON_NAV;
                    }
//                    stopMotors();
//                    delay(20000);
//                    senseLine();
//                    if (lineSensorState == NO_ACTIVE) {
//                        robotTaskState = CANYON_NAV;
//                    }
                }
                
                
                
                if ((sampleReturned == 1) && (FAR_LEFT_LINE_SIG < LINE_SENSOR_THRESHOLD) && (CENTER_LINE_SIG < LINE_SENSOR_THRESHOLD)) {
                        stopMotors();
                        delay(20000);
                        robotTaskState = DATA_TRANSMIT;
                }                


                
                if ((sampleCollected == 0) && (SAMPLE_IR_SIG > IR_SIG_THRESH)) {
                        robotTaskState = SAMPLE_COLLECT;
                } 
                
                
                if (RIGHT_SHARP_SIG > RIGHT_SHARP_THRESH_SAMPLE_RETURN) {
                    if ((sampleCollected == 1) && (sampleReturned == 0)) {
                        robotTaskState = SAMPLE_RETURN;
                    }
                }


                break;
                
                
            case CANYON_NAV:
                
                canyonNav();
                
                if (senseLineEndOfTask()) {
                    turnRightGetOnLine();
                    robotTaskState = LINE_FOLLOW;
                    OC2RS = PERIOD;
                    OC3RS = PERIOD;
                    OC2R = PERIOD/2;
                    OC3R = PERIOD/2;
                }
                
                break;
                
                
            case SAMPLE_COLLECT:
                stopMotors();
                collectSample();
                robotTaskState = LINE_FOLLOW;
                sampleCollected = 1;
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
                delay(5000); 
                robotTaskState = LINE_FOLLOW;
                sampleReturned = 1;
                break;
                
                
            case DATA_TRANSMIT:
                goStraight(HALF_SPEED);  //sets speed
                stopMotors();
                moveForward(200);
                turnLeft();
//                while (!(Collision())){
//                    lineNav();
//                }
//                stopMotors();
                moveForward(2000);
                pointLaser();
                break;
        }

    }
    
    return 0;
}
