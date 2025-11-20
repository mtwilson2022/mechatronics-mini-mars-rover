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

void __attribute__((interrupt, no_auto_psv)) _OC3Interrupt(void) {
    // OC3 Interrupt Service Routine MOTOR_TWO
    _OC3IF = 0;
    motorSteps += 1;
}

//------------------------------------------------
// ********** General purpose functions **********
//------------------------------------------------

void delay(long s) {
    long k = 0;
    while (k < s) {
        k++;
    }
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

//----------------------------------------------
//********** Line sensing & following **********
//----------------------------------------------

void lineNav() {
    switch (lineSensorState) {
            case GO_CENTER:
                OC2RS = PERIOD;
                OC3RS = PERIOD;
                OC2R = DUTY;
                OC3R = DUTY;
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


void senseLine(void)    {
    if (
            !((RIGHT_LINE_SIG < LINE_SENSOR_THRESHOLD) 
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
                
                goStraight();

                if (Collision()) {
                    canyonSensorState = RIGHT;
                }
                
                break;
                
            case TURN_AROUND:
                LEFT_LED = 1;
                STRAIGHT_LED = 0;
                RIGHT_LED = 0;
               
                turnAround();
                delay(40000);

                canyonSensorState = STRAIGHT;
                
                break;
                
            case RIGHT:

                LEFT_LED = 0;
                STRAIGHT_LED = 0;
                RIGHT_LED = 1;

                turnRight();
                delay(40000);
                
                if (Collision()) {
                    canyonSensorState = TURN_AROUND;
                }
                else {
                    canyonSensorState = STRAIGHT;
                }

                break;
        }

}


void turnAround() {
    
    _OC2IE = 0; //stop counting steps
    OC2R = 0; //stop motor
    OC3R = 0;

    DIRECTION_MOTOR_ONE = 1; //change direction for turn
    DIRECTION_MOTOR_TWO = 1;
    motorSteps = 0;
    stepsNeeded = 1200;
    _OC2IE = 1; //enable counting steps again
    OC2R = DUTY; //will start motor
    OC3R = DUTY;
    
    while (motorSteps <= stepsNeeded) {
        continue;
    }
}


void turnRight() {
    
    _OC2IE = 0; //stop counting steps
    OC2R = 0; //stop motor
    OC3R = 0;

    DIRECTION_MOTOR_ONE = 0; //change direction for turn
    DIRECTION_MOTOR_TWO = 0;
    motorSteps = 0;
    stepsNeeded = 650;
    _OC2IE = 1; //enable counting steps again
    OC2R = DUTY; //will start motor
    OC3R = DUTY;
    
    while (motorSteps <= stepsNeeded) {
        continue;
    }
}


void turnLeft() {
    
    _OC2IE = 0; //stop counting steps
    OC2R = 0; //stop motor
    OC3R = 0;

    DIRECTION_MOTOR_ONE = 1; //change direction for turn
    DIRECTION_MOTOR_TWO = 1;
    motorSteps = 0;
    stepsNeeded = 650;
    _OC2IE = 1; //enable counting steps again
    OC2R = DUTY; //will start motor
    OC3R = DUTY;
    
    while (motorSteps <= stepsNeeded) {
        continue;
    }
}


void goStraight() {
    _OC2IE = 0;
    OC2R = DUTY;
    OC3R = DUTY;
    DIRECTION_MOTOR_ONE = 0;
    DIRECTION_MOTOR_TWO = 1;
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
    OC2R = 0;
    OC3R = 0;
    delay(20000);
    turnLeft();
    OC2R = 0;
    OC3R = 0;
    delay(20000);
    goStraight();
}


/*
 * Similar to depositBlackBall(), but brings the robot to the white ball return.
 */
void depositWhiteBall() {
    _LATB7 = 1;
    _LATB8 = 0;
    _LATB9 = 0;
}


//---------------------------------------------
//********** Data transmission tasks **********
//---------------------------------------------
