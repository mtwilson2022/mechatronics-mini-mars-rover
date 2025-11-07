/* 
 * File:   Configurations.h
 * Author: Matthew Wilson
 * Co-authors: William Bowler, Caden Meyer, Sean Reisinger
 * Created for an ME EN 330 "Mars Rover" project
 * 
 * Contains setup code for a PIC24F microcontroller in a class project.
 * Includes define statements and prototypes of 
 * setup functions for I/O, PWM, ADC.
 * Does not include #pragma config commands, which are given in the main file.
 * 
 * Created on November 6, 2025, 10:12 AM
 */

#ifndef CONFIGURATIONS_H
#define	CONFIGURATIONS_H

//************ Define statements ************
#define BUTTON _RB4             //temporary button to start sequence for milestone 6
#define DIRECTION_MOTOR_ONE _LATA0      //left motor
#define DIRECTION_MOTOR_TWO _LATA1      //right motor
#define DEBUG_LED _LATB8
#define PERIOD 312*1/2              //@F_cy = 250kHz, 1rev/s in 1/4 step mode = period of 312
#define DUTY PERIOD/2


//************ Enums for state machines ************


//************ Function prototypes ************
void configurePins();
void setupMotors();
void configAD();


#endif	/* CONFIGURATIONS_H */

