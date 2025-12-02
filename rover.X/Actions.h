/* 
 * File:   Actions.h
 * Authors: Matthew Wilson, Sean Reisinger, William Bowler, Caden Meyer
 * Created for an ME EN 330 "Mars Rover" project
 * 
 * Functions governing the rover's movement around the track, including:
    * Line following
    * Canyon navigation
    * Transition between line following and canyon states
 * 
 * Created on November 18, 2025, 5:59 PM
 */

#ifndef ACTIONS_H
#define	ACTIONS_H

#include <xc.h>
#include "Configurations.h"

//************ Function prototypes ************
// General
void delay(long s);
void stopMotors();
void startMotors();
void goStraight(int speed);
void turnAround();
void turnRight();
void turnLeft();

// Transitions between tasks
void moveIntoCanyon(); // this one is maybe not necessary
int senseLineEndOfCanyon();


// Line Following
void lineNav();
void senseLine();

// Canyon navigation
void canyonNav();
int senseWallRight();
int Collision();

// Sample collection


// Sample return
int senseBallWhite();
void depositBlackBall();
void depositWhiteBall();

// Data transmission


#endif	/* ACTIONS_H */

