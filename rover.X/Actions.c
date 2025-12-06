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
extern RobotTaskState robotTaskState;
extern CanyonSensorState canyonSensorState;
extern LineSensorState lineSensorState;

// Global variables for turning functions. (Maybe put these in header file?)
int motorSteps = 0;          //step counter for stepper motor
int stepsNeeded = 0;         // Desired steps for motor(s))
int foundLine = 0;


//----------
//Interrupts
//----------

// OC2 Interrupt Service Routine MOTOR_ONE
void __attribute__((interrupt, no_auto_psv)) _OC2Interrupt(void) {
    _OC2IF = 0;
    motorSteps += 1;
}

void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void) {
    _OC1IF = 0;
    SERVO_ANGLE += 1;
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

void stopMotors() {
    OC2R = 0;
    OC3R = 0;
}

void startMotors() {
    OC2R = DUTY;
    OC3R = DUTY;
}

void setDirectionRight(){
    DIRECTION_MOTOR_ONE = 0;
    DIRECTION_MOTOR_TWO = 0;
}

void setDirectionLeft(){
    DIRECTION_MOTOR_ONE = 1;
    DIRECTION_MOTOR_TWO = 1;
}

void setDirectionStraight(){
    DIRECTION_MOTOR_ONE = 0;
    DIRECTION_MOTOR_TWO = 1;
}

/* in the speed parameter, pass in one of the following #define statements: 
 * (from the Configurations.h file)
 * FULL_SPEED
 * CANYON_SPEED
*/
void goStraight(int speed) {
    if (speed == FULL_SPEED) {
        // normally, go at full speed (e.g. during line following)
        OC2RS = PERIOD;
        OC3RS = PERIOD;
    }
    else if (speed == HALF_SPEED) {
        // go at half speed while in the canyon
        OC2RS = PERIOD * 2;
        OC3RS = PERIOD * 2;
    }
    
    _OC2IE = 0;
    DIRECTION_MOTOR_ONE = 0;
    DIRECTION_MOTOR_TWO = 1;
    OC2R = DUTY;
    OC3R = DUTY;
}


/*
 * Executes a 180 degree turn.
 */
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
    // stop motors at the end of it
    stopMotors();
    delay(5000);
}


/*
 * Executes a 90 degree right turn.
 */
void turnRight() {
    _OC2IE = 0; //stop counting steps
    stopMotors();
    DIRECTION_MOTOR_ONE = 0; //change direction for turn
    DIRECTION_MOTOR_TWO = 0;
    motorSteps = 0;
    stepsNeeded = RIGHT_TURN_STEPS;
    _OC2IE = 1; //enable counting steps again
    startMotors();
    while (motorSteps <= stepsNeeded) {
        continue;
    }
    // stop motors at the end of it
    stopMotors();
    delay(5000);
}

/*
 * Executes a 90 degree left turn.
 */
void turnLeft() {
    _OC2IE = 0; //stop counting steps
    stopMotors();
    DIRECTION_MOTOR_ONE = 1; //change direction for turn
    DIRECTION_MOTOR_TWO = 1;
    motorSteps = 0;
    stepsNeeded = LEFT_TURN_STEPS;
    _OC2IE = 1; //enable counting steps again
    startMotors();
    while (motorSteps <= stepsNeeded) {
        continue;
    }
    // stop motors at the end of it
    stopMotors();
    delay(5000);
}

// 45 degree right turn
void turnSlightRight() {
    _OC2IE = 0; //stop counting steps
    stopMotors();
    DIRECTION_MOTOR_ONE = 0; //change direction for turn
    DIRECTION_MOTOR_TWO = 0;
    motorSteps = 0;
    stepsNeeded = RIGHT_TURN_STEPS/2;
    _OC2IE = 1; //enable counting steps again
    startMotors();
    while (motorSteps <= stepsNeeded) {
        continue;
    }
    // stop motors at the end of it
    stopMotors();
    delay(5000);
}

//45 degree left turn
void turnSlightLeft() {
    _OC2IE = 0; //stop counting steps
    stopMotors();
    DIRECTION_MOTOR_ONE = 1; //change direction for turn
    DIRECTION_MOTOR_TWO = 1;
    motorSteps = 0;
    stepsNeeded = LEFT_TURN_STEPS/2;
    _OC2IE = 1; //enable counting steps again
    startMotors();
    while (motorSteps <= stepsNeeded) {
        continue;
    }
    // stop motors at the end of it
    stopMotors();
    delay(5000);
}



/*
 * Move forward a preset number of steps.
 * Useful for sample collection and return
 */
void moveForward(int stepsNeeded) {
    stopMotors();
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
}


void moveBackward(int stepsNeeded) {
    stopMotors();
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
}

//--------------------------------------------------------------------
// ********** Functions for transitions between task states **********
//--------------------------------------------------------------------

int senseLineEndOfTask() {
    if (   (RIGHT_LINE_SIG < LINE_SENSOR_THRESHOLD) 
        && (CENTER_LINE_SIG < LINE_SENSOR_THRESHOLD) 
        && (LEFT_LINE_SIG < LINE_SENSOR_THRESHOLD) ) {
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
    delay(5000);
//    moveBackward(200);
    
    // make a ~30 degree right turn
    _OC2IE = 0; //stop counting steps
    stopMotors();
    DIRECTION_MOTOR_ONE = 0; //change direction for turn
    DIRECTION_MOTOR_TWO = 0;
    motorSteps = 0;
    stepsNeeded = 300;
    _OC2IE = 1; //enable counting steps again
    startMotors();
    while (motorSteps <= stepsNeeded) {
        continue;
    }
    // stop motors at the end of it
    stopMotors();
    delay(5000);
    
    while (1) {
        goStraight(HALF_SPEED);
        if (RIGHT_LINE_SIG < LINE_SENSOR_THRESHOLD) {
            break;
        }
    }   
}

/* 
 * Helps the rover turn left, then make sure it's on the line again, before
 * resuming line following.
 */
void turnLeftGetOnLine() {
    stopMotors();
    delay(5000);
    moveBackward(200);
    
    // make a ~30 degree right turn
    _OC2IE = 0; //stop counting steps
    stopMotors();
    DIRECTION_MOTOR_ONE = 1; //change direction for turn
    DIRECTION_MOTOR_TWO = 1;
    motorSteps = 0;
    stepsNeeded = 200;
    _OC2IE = 1; //enable counting steps again
    startMotors();
    while (motorSteps <= stepsNeeded) {
        continue;
    }
    // stop motors at the end of it
    stopMotors();
    delay(5000);
    
    while (1) {
        goStraight(HALF_SPEED);
        senseLine();
        if (!(lineSensorState == NO_ACTIVE)){
            break;
        }
    }     
}

//----------------------------------------------
//********** Line sensing & following **********
//----------------------------------------------

void lineNav() {
    switch (lineSensorState) {
            case CENTER:
                goStraight(FULL_SPEED);
                senseLine();
                break;
            
            case LEFT_CENTER:
                OC2RS = PERIOD;
                OC3RS = PERIOD*3/2;
                OC2R = DUTY;    //OC2 is the right motor
                OC3R = DUTY;
                senseLine();
                break;
                
            case LEFT:
                OC2RS = PERIOD;
                OC3RS = PERIOD*4;
                OC2R = DUTY;    //OC2 is the right motor
                OC3R = DUTY;
                senseLine();
                break;
                
            case RIGHT_CENTER:
                OC2RS = PERIOD*3/2;
                OC3RS = PERIOD;
                OC2R = DUTY;       //OC2 is the right motor
                OC3R = DUTY;
                senseLine();
                break;
                
            case RIGHT:
                OC2RS = PERIOD*4;
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


void senseLine() {    
    if (!(   (RIGHT_LINE_SIG < LINE_SENSOR_THRESHOLD) 
          || (CENTER_LINE_SIG < LINE_SENSOR_THRESHOLD) 
          || (LEFT_LINE_SIG < LINE_SENSOR_THRESHOLD))) 
    {
        lineSensorState = NO_ACTIVE;
    }
    else if ((RIGHT_LINE_SIG < LINE_SENSOR_THRESHOLD) 
            && (LEFT_LINE_SIG < LINE_SENSOR_THRESHOLD)) 
    {
        lineSensorState = CENTER;
    }
    else if ((RIGHT_LINE_SIG < LINE_SENSOR_THRESHOLD)
            && (CENTER_LINE_SIG < LINE_SENSOR_THRESHOLD)) {
        lineSensorState = RIGHT_CENTER;
    }
    else if ((RIGHT_LINE_SIG < LINE_SENSOR_THRESHOLD)
            && !(CENTER_LINE_SIG < LINE_SENSOR_THRESHOLD)) {
        lineSensorState = RIGHT;
    }
    else if ((LEFT_LINE_SIG < LINE_SENSOR_THRESHOLD)
            && (CENTER_LINE_SIG < LINE_SENSOR_THRESHOLD))
    {
        lineSensorState = LEFT_CENTER;
    }
    else if ((LEFT_LINE_SIG < LINE_SENSOR_THRESHOLD)
            && !(CENTER_LINE_SIG < LINE_SENSOR_THRESHOLD))
    {
        lineSensorState = LEFT;
    }
    else if (CENTER_LINE_SIG < LINE_SENSOR_THRESHOLD) {
        lineSensorState = CENTER;
    }
}

//---------------------------------------------------
//********** Canyon sensing and navigating **********
//---------------------------------------------------

void canyonNav() {
    switch (canyonSensorState) {
            
            case STRAIGHT:
                goStraight(HALF_SPEED);

                if (Collision()) {
                    stopMotors();
                    delay(5000);
                    if (senseWallRight() && Collision()) {
                        canyonSensorState = WALL_RIGHT;
                    }
                    else if (Collision()){
                        canyonSensorState = WALL_LEFT;
                    }
                }
                
                break;
                
            case WALL_RIGHT:
                stopMotors();
                delay(5000);
                OC2RS = PERIOD * 4;
                OC3RS = PERIOD * 4;
                turnLeft();
                canyonSensorState = STRAIGHT;
                
                break;
                
            case WALL_LEFT:
                stopMotors();
                delay(5000);
                OC2RS = PERIOD * 4;
                OC3RS = PERIOD * 4;
                turnRight();
                canyonSensorState = STRAIGHT;

                break;
        }

}


int senseWallRight() {
    if (RIGHT_SHARP_SIG > RIGHT_SHARP_THRESH) {
        return 1;
    }
    return 0;
}


int Collision() {
    if (FRONT_SHARP_SIG > FRONT_SHARP_THRESH) {
        return 1;
    }
    return 0;
}

//----------------------------------------------------------------
// ********** Sample collection: sensing and collecting **********
//----------------------------------------------------------------

void collectSample() {
    turnRight();
    goStraight(HALF_SPEED); // slow down the robot so it doesn't crash
    stopMotors();
    moveForward(400); // number of steps to push the wall to get the sample
    delay(20000);
    moveBackward(400);
    delay(20000);
    turnLeft();
    

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
    moveForward(400);
    
    // turn servo to deposit ball
    OC1R = DROP_BALL;
    delay(20000);
    OC1R = BLOCK_BALL;
    
    moveBackward(400);
    turnLeft();
    
    // may need something to ensure it gets on the line before proceeding
    goStraight(FULL_SPEED);
}

/*
 * Similar to depositBlackBall(), but brings the robot to the white ball return.
 */
void depositWhiteBall() {
    _LATB7 = 1;
    _LATB8 = 0;
    _LATB9 = 0;
    
    moveForward(600);
    turnLeft();
    
    moveForward(400);

    
    // turn servo to deposit ball
    OC1R = DROP_BALL;
    delay(20000);
    OC1R = BLOCK_BALL;
    
    moveBackward(400);
    turnRight();
    
    // may need something to ensure it gets on the line before proceeding
    goStraight(FULL_SPEED);
}

//---------------------------------------------
//********** Data transmission tasks **********
//---------------------------------------------

void returnHome() {
    
}

void pointLaser() {
     _OC1IE = 1; // interrupt is taking care of the servo motion
            
    while (1) {
        
        if (TRANSMIT > TRANSMIT_THRESHOLD) {
            _OC1IE = 0;
            LASER = 1;
        }
        else if (SERVO_ANGLE >= VERTICAL) {
            SERVO_ANGLE = HORIZONTAL;
        }
    }
}

//if robot goes off the line during lineNav, this will turn right then left slightly to try to find the line again.
//If does not find a line, will enter canyonNav.
int checkOffLine() {
    stopMotors();
    foundLine = 0;
    OC2RS = PERIOD * 4; //set speed to 1/4th
    OC3RS = PERIOD * 4;
    if (checkRight()){
        foundLine = 1;
    }
    if (foundLine == 0){
        if (checkLeft()){
            foundLine = 1;
        }
    }
    OC2RS = PERIOD; //reset speed to not interfere with lineNav.
    OC2RS = PERIOD;
    if (foundLine == 1){
        return 0;
    }
    return 1;
}

int checkRight(){
    delay(5000);
    setDirectionRight();
    _OC2IE = 1;
    motorSteps = 0;
    startMotors();
    while (motorSteps <= RIGHT_TURN_STEPS/2){   //turns up to 45degree, sensing for line.
        senseLine();
        if (lineSensorState != NO_ACTIVE){
            _OC2IE = 0;
            stopMotors();
            delay(5000);   //delay and double check to ensure sensor is correctly sensing.
            if (lineSensorState != NO_ACTIVE){
                return 1;               //return 1 if a line is sensed. Robot is now stopped on the line.
            }                     //continues if double-check shows false positive
            _OC2IE = 1;
            startMotors();
        }
    }
    stopMotors();
    delay(5000);
    setDirectionLeft();
    motorSteps = 0;
    startMotors();
    while (motorSteps <= LEFT_TURN_STEPS/2){   //returns robot to front face if no line sensed
        continue;
    }
    stopMotors();
    _OC2IE = 0;
    return 0;
}

int checkLeft(){
    delay(5000);
    setDirectionLeft();
    _OC2IE = 1;
    motorSteps = 0;
    startMotors();
    while (motorSteps <= LEFT_TURN_STEPS/2){   //turns up to 45degree, sensing for line.
        senseLine();
        if (lineSensorState != NO_ACTIVE){
            _OC2IE = 0;
            stopMotors();
            delay(5000);   //delay and double check to ensure sensor is correctly sensing.
            if (lineSensorState != NO_ACTIVE){
                return 1;               //return 1 if a line is sensed. Robot is now stopped on the line.
            }                     //continues if double-check shows false positive
            _OC2IE = 1;
            startMotors();
        }
    }
    stopMotors();
    delay(5000);
    setDirectionRight();
    motorSteps = 0;
    startMotors();
    while (motorSteps <= RIGHT_TURN_STEPS/2){   //returns robot to front face if no line sensed
        continue;
    }
    stopMotors();
    _OC2IE = 0;
    return 0;
}


