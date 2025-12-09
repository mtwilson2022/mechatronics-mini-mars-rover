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
#define DIRECTION_MOTOR_ONE _LATB8      //left motor
#define DIRECTION_MOTOR_TWO _LATB9      //right motor

#define PERIOD 312/2             //@F_cy = 250kHz, 1rev/s in 1/4 step mode = period of 312
#define DUTY PERIOD*0.5

#define FULL_SPEED 0 // used when setting speed of the motors
#define HALF_SPEED 1
#define QUARTER_SPEED 2
#define RIGHT_TURN_STEPS 557
#define LEFT_TURN_STEPS 557

#define SERVO_PERIOD 4999
#define BLOCK_BALL 250
#define DROP_BALL 500

// Laser
#define TRANSMIT ADC1BUF15
#define TRANSMIT_THRESHOLD 500
#define LASER _LATA4

#define SERVO_PERIOD 4999
#define SERVO_ANGLE OC1R
#define HORIZONTAL 250
#define VERTICAL 500

// Sensing

#define RIGHT_SHARP_SIG ADC1BUF13
#define LEFT_SHARP_SIG ADC1BUF0
#define FRONT_SHARP_SIG ADC1BUF4
#define FRONT_SHARP_THRESH 1000
#define FRONT_SHARP_THRESH_START 1000
#define RIGHT_SHARP_THRESH 1000
#define RIGHT_SHARP_THRESH_SAMPLE_RETURN 700

#define SAMPLE_IR_SIG ADC1BUF14
#define IR_SIG_THRESH  700

#define FAR_LEFT_LINE_SIG ADC1BUF1
#define LEFT_LINE_SIG  ADC1BUF10
#define RIGHT_LINE_SIG  ADC1BUF12
#define CENTER_LINE_SIG  ADC1BUF11
#define BALL_COLOR_SIG  ADC1BUF9
#define LINE_SENSOR_THRESHOLD 2048



//************ Global variables ************
// example: extern int hasBall;  // initialize in the main file


//************ Enums for state machines ************

// state determining the current task the robot should perform
typedef enum {LINE_FOLLOW, CANYON_NAV, SAMPLE_COLLECT, SAMPLE_RETURN, DATA_TRANSMIT} RobotTaskState;
// determining actions the robot should perform while in the canyon
typedef enum {STRAIGHT, WALL_RIGHT, WALL_LEFT} CanyonSensorState;
// determining the robot's actions while following the line
typedef enum {LEFT, LEFT_CENTER, CENTER, RIGHT_CENTER, RIGHT, NO_ACTIVE} LineSensorState;


#endif	/* CONFIGURATIONS_H */
