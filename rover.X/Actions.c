/* 
 * File:   Actions.c
 * Authors: Matthew Wilson, Sean Reisinger, William Bowler, Caden Meyer
 * Created for an ME EN 330 "Mars Rover" project
 * 
 * Functions governing the rover's movement around the track, including:
    * Line following
    * Canyon navigation
    * Transition between line following and canyon states
 * 
 * 
 * Created on November 6, 2025, 10:12 AM
 */


#include <xc.h>
#include "Configurations.h"
#include "Actions.h"

// state machines that are initialized in the main file, but altered in 
// the functions contained here
extern CanyonSensorState canyonSensorState;
extern LineSensorState lineSensorState;

// Global variables for turning functions. (Maybe put these in header file?)
int motorSteps = 0;          //step counter for stepper motor
int stepsNeeded = 0;         // Desired steps for motor(s))


//----------
//Interrupts
//----------

// OC2 Interrupt Service Routine MOTOR_ONE
void __attribute__((interrupt, no_auto_psv)) _OC2Interrupt(void) {
    _OC2IF = 0;
    motorSteps += 1;
}

//// OC3 Interrupt Service Routine MOTOR_TWO
//void __attribute__((interrupt, no_auto_psv)) _OC3Interrupt(void) {
//    _OC3IF = 0;
//    motorSteps += 1;
//}

//------------------------------------------------
// ********** General purpose functions **********
//------------------------------------------------

void delay(long s) {
    long k = 0;
    while (k < s) {
        k++;
    }
}

void stopMotors() {
    OC2R = 0;
    OC3R = 0;
}

void startMotors() {
    OC2R = DUTY;
    OC3R = DUTY;
}


void goStraight(int speed) {
    if (speed == FULL_SPEED) {
        // normally, go at full speed (e.g. during line following)
        OC2RS = PERIOD;
        OC3RS = PERIOD;
    }
    else if (speed == CANYON_SPEED) {
        // go at half speed while in the canyon
        OC2RS = PERIOD * 2;
        OC3RS = PERIOD * 2;
    }
    
    _OC2IE = 0;
    OC2R = DUTY;
    OC3R = DUTY;
    DIRECTION_MOTOR_ONE = 0;
    DIRECTION_MOTOR_TWO = 1;
}


void turnAround() {
    
    _OC2IE = 0; //stop counting steps
    OC2R = 0; //stop motor
    OC3R = 0;

    DIRECTION_MOTOR_ONE = 1; //change direction for turn
    DIRECTION_MOTOR_TWO = 1;
    motorSteps = 0;
    stepsNeeded = 1150;
    _OC2IE = 1; //enable counting steps again
    OC2R = DUTY; //will start motor
    OC3R = DUTY;
    
    while (motorSteps <= stepsNeeded) {
        continue;
    }
}


void turnRight() {
    
    _OC2IE = 0; //stop counting steps
    stopMotors();

    DIRECTION_MOTOR_ONE = 0; //change direction for turn
    DIRECTION_MOTOR_TWO = 0;
    motorSteps = 0;
    stepsNeeded = 575;
    _OC2IE = 1; //enable counting steps again
    startMotors();
    
    while (motorSteps <= stepsNeeded) {
        continue;
    }
    
    // stop motors at the end of it
    stopMotors();
    delay(5000);
}


void turnLeft() {
    
    _OC2IE = 0; //stop counting steps
    stopMotors();

    DIRECTION_MOTOR_ONE = 1; //change direction for turn
    DIRECTION_MOTOR_TWO = 1;
    motorSteps = 0;
    stepsNeeded = 575;
    _OC2IE = 1; //enable counting steps again
    startMotors();
    
    while (motorSteps <= stepsNeeded) {
        continue;
    }
    // stop motors at the end of it
    stopMotors();
    delay(5000);
}

//--------------------------------------------------------------------
// ********** Functions for transitions between task states **********
//--------------------------------------------------------------------

int senseLineEndOfCanyon() {
    if (   (RIGHT_LINE_SIG < LINE_SENSOR_THRESHOLD) 
        || (CENTER_LINE_SIG < LINE_SENSOR_THRESHOLD) 
        || (LEFT_LINE_SIG < LINE_SENSOR_THRESHOLD) ) {
        return 1;
    }
    return 0;
}


/*
 * When the rover senses the line, it needs to orient itself correctly 
 * to ensure that it follows the line in the correct direction. This function
 * helps the rover turn right, then make sure it's on the line again, before
 * resuming line following.
 */
void turnRightGetOnLine() {
    stopMotors();
    turnRight();
}

/* 
 * Helps the rover turn left, then make sure it's on the line again, before
 * resuming line following.
 */
void turnLeftGetOnLine() {
    
}

//----------------------------------------------
//********** Line sensing & following **********
//----------------------------------------------

void lineNav() {
    switch (lineSensorState) {
            case GO_CENTER:
                goStraight(FULL_SPEED);
                senseLine();
                break;
            
            case GO_LEFT:
                OC2RS = PERIOD;
                OC3RS = PERIOD*3;
                OC2R = DUTY;    //OC2 is the right motor
                OC3R = DUTY;
                senseLine();
                break;
                
            case GO_RIGHT:
                OC2RS = PERIOD*3;
                OC3RS = PERIOD;
                OC2R = DUTY;       //OC2 is the right motor
                OC3R = DUTY;
                senseLine();
                break;
                
            case NO_ACTIVE:
                OC2RS = PERIOD;
                OC3RS = PERIOD;
                OC2R = 0;
                OC3R = 0;
                senseLine();
                break;
        }
}


void senseLine()    {
    if (!(   (RIGHT_LINE_SIG < LINE_SENSOR_THRESHOLD) 
          || (CENTER_LINE_SIG < LINE_SENSOR_THRESHOLD) 
          || (LEFT_LINE_SIG < LINE_SENSOR_THRESHOLD))) 
    {
        lineSensorState = NO_ACTIVE;
    }
    else if ((RIGHT_LINE_SIG < LINE_SENSOR_THRESHOLD) 
            && (LEFT_LINE_SIG < LINE_SENSOR_THRESHOLD)) 
    {
        lineSensorState = GO_CENTER;
    }
    else if ((RIGHT_LINE_SIG < LINE_SENSOR_THRESHOLD)) {
        lineSensorState = GO_RIGHT;
    }
    else if ((LEFT_LINE_SIG < LINE_SENSOR_THRESHOLD)){
        lineSensorState = GO_LEFT;
    }
    else if (CENTER_LINE_SIG < LINE_SENSOR_THRESHOLD) {
        lineSensorState = GO_CENTER;
    }
}

//---------------------------------------------------
//********** Canyon sensing and navigating **********
//---------------------------------------------------

void canyonNav() {
    switch (canyonSensorState) {
            
            case STRAIGHT:

                STRAIGHT_LED = 1;
                LEFT_LED = 0;
                RIGHT_LED = 0;
                
                goStraight(CANYON_SPEED);

                if (Collision()) {
                    if (senseWallRight()) {
                        canyonSensorState = LEFT;
                    }
                    else {
                        canyonSensorState = RIGHT;
                    }
                }
                
                break;
                
            case LEFT:
                LEFT_LED = 1;
                STRAIGHT_LED = 0;
                RIGHT_LED = 0;
               
                stopMotors();
                delay(5000);
                turnLeft();

                canyonSensorState = STRAIGHT;
                
                break;
                
            case RIGHT:

                LEFT_LED = 0;
                STRAIGHT_LED = 0;
                RIGHT_LED = 1;
                
                stopMotors();
                delay(5000);
                turnRight();
                
                canyonSensorState = STRAIGHT;

                break;
        }

}


int senseWallRight() {
    if (RIGHT_SONAR_SIG < SONAR_THRESHOLD) {
        return 1;
    }
    return 0;
}


int Collision() {
    if (FRONT_SONAR_SIG < SONAR_THRESHOLD) {
        return 1;
    }
    return 0;
}

//----------------------------------------------------------------
// ********** Sample collection: sensing and collecting **********
//----------------------------------------------------------------

void collectSample() {
    turnRight();
    
    // go forward
    stepsNeeded = 400;
    motorSteps = 0;
    _OC2IE = 0;
    DIRECTION_MOTOR_ONE = 0;
    DIRECTION_MOTOR_TWO = 1;
    startMotors();
    _OC2IE = 1;
    while (motorSteps <= stepsNeeded) {
        continue;
    }
    stopMotors();
}


//------------------------------------------------------------
// ********** Sample return: sensing and depositing **********
//------------------------------------------------------------

/*
 * If the ball in the collection ramp is white, returns true. If the ball is
 * black, returns false.
 */
int senseBallWhite() {
    if (BALL_COLOR_SIG < LINE_SENSOR_THRESHOLD) {
        return 1;
    }
    return 0;
}


/*
 * This function brings the robot directly in front of the black ball return 
 * box, turns the servo to release the ball into the box, and returns the 
 * robot to the line, whereupon it resumes line following.
 */
void depositBlackBall() {
    _LATB7 = 0;
    _LATB8 = 0;
    _LATB9 = 1;
    
    turnRight();
    
    // go forward
    stepsNeeded = 400;
    motorSteps = 0;
    _OC2IE = 0;
    DIRECTION_MOTOR_ONE = 0;
    DIRECTION_MOTOR_TWO = 1;
    startMotors();
    _OC2IE = 1;
    while (motorSteps <= stepsNeeded) {
        continue;
    }
    stopMotors();
    
    // turn servo to deposit ball
    delay(5000);
    OC1R = DROP_BALL;
    delay(20000);
    OC1R = BLOCK_BALL;
    
    // go backward
    motorSteps = 0;
    _OC2IE = 0;
    DIRECTION_MOTOR_ONE = 1;
    DIRECTION_MOTOR_TWO = 0;
    startMotors();
    _OC2IE = 1;
    while (motorSteps <= stepsNeeded) {
        continue;
    }
    stopMotors();
    
    delay(5000);
    turnLeft();
    
    goStraight(FULL_SPEED);
}


/*
 * Similar to depositBlackBall(), but brings the robot to the white ball return.
 */
void depositWhiteBall() {
    _LATB7 = 1;
    _LATB8 = 0;
    _LATB9 = 0;
    
    // go forward
    delay(5000);
    stepsNeeded = 600;
    motorSteps = 0;
    _OC2IE = 0;
    DIRECTION_MOTOR_ONE = 0;
    DIRECTION_MOTOR_TWO = 1;
    startMotors();
    _OC2IE = 1;
    while (motorSteps <= stepsNeeded) {
        continue;
    }
    stopMotors();
    delay(5000);
    
    turnLeft();
    
    // go forward
    stepsNeeded = 400;
    motorSteps = 0;
    _OC2IE = 0;
    DIRECTION_MOTOR_ONE = 0;
    DIRECTION_MOTOR_TWO = 1;
    startMotors();
    _OC2IE = 1;
    while (motorSteps <= stepsNeeded) {
        continue;
    }
    stopMotors();
    
    // turn servo to deposit ball
    delay(5000);
    OC1R = DROP_BALL;
    delay(20000);
    OC1R = BLOCK_BALL;
    
    // go backward
    motorSteps = 0;
    _OC2IE = 0;
    DIRECTION_MOTOR_ONE = 1;
    DIRECTION_MOTOR_TWO = 0;
    startMotors();
    _OC2IE = 1;
    while (motorSteps <= stepsNeeded) {
        continue;
    }
    stopMotors();
    
    delay(5000);
    turnRight();
    
    goStraight(FULL_SPEED);
}

//---------------------------------------------
//********** Data transmission tasks **********
//---------------------------------------------
