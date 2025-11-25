/* 
 * File:   Configurations.h
 * Authors: Matthew Wilson, Sean Reisinger, William Bowler, Caden Meyer
 * Created for an ME EN 330 "Mars Rover" project
 * 
 * Contains setup code for a PIC24F microcontroller in a class project.
 * Includes define statements and enums used for rover setup and actions.
 * Does not include #pragma config commands, which are given in the main file.
 * 
 * Created on November 6, 2025, 10:12 AM
 */

#ifndef CONFIGURATIONS_H
#define	CONFIGURATIONS_H

//************ Define statements ************

// PWM
#define DIRECTION_MOTOR_ONE _LATA0      //left motor
#define DIRECTION_MOTOR_TWO _LATA1      //right motor

#define PERIOD 312/2             //@F_cy = 250kHz, 1rev/s in 1/4 step mode = period of 312
#define DUTY PERIOD*0.5

#define FULL_SPEED 0 // used when setting speed of the motors
#define CANYON_SPEED 1

#define SERVO_PERIOD 4999
#define BLOCK_BALL 250
#define DROP_BALL 500

// Sensing

#define RIGHT_SONAR_SIG ADC1BUF13
#define FRONT_SONAR_SIG ADC1BUF4
#define SONAR_THRESHOLD 850  // 850 for 3.3 V; 1280 for 5 V probably gives same result

#define IR_SIG ADC1BUF14
#define IR_SIG_THRESH  500

#define LEFT_LINE_SIG  ADC1BUF10
#define RIGHT_LINE_SIG  ADC1BUF12
#define CENTER_LINE_SIG  ADC1BUF11
#define BALL_COLOR_SIG  ADC1BUF9
#define LINE_SENSOR_THRESHOLD 2048

// Debugging
#define STRAIGHT_LED _LATB8
#define LEFT_LED _LATB7
#define RIGHT_LED _LATB9


//************ Global variables ************
// example: extern int hasBall;  // initialize in the main file

//************ Enums for state machines ************

// NOTE: ALWAYS DECLARE THE ENUM AS STATIC!!

// state determining the current task the robot should perform
static enum {LINE_FOLLOW, CANYON_NAV, SAMPLE_COLLECT, SAMPLE_RETURN} robotTaskState;
// determining actions the robot should perform while in the canyon
static enum {STRAIGHT, RIGHT, TURN_AROUND} canyonSensorState;
// determining the robot's actions while following the line
static enum {GO_CENTER, GO_LEFT, GO_RIGHT, NO_ACTIVE} lineSensorState;


#endif	/* CONFIGURATIONS_H */

