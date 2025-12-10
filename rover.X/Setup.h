
/* 
 * File:   Setup.h
 * Authors: Matthew Wilson, Sean Reisinger, William Bowler, Caden Meyer
 * Created for an ME EN 330 "Mars Rover" project
 * 
 * Prototypes of setup functions for I/O, PWM, ADC.
 *
 * Created on November 18, 2025, 6:09 PM
 */

#ifndef SETUP_H
#define    SETUP_H

#include <xc.h>
#include "Configurations.h"

//************ Function prototypes ************
void configurePins();
void setupMotors();
void configAD();


#endif    /* SETUP_H */
