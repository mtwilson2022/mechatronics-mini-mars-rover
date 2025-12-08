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
void setDirectionRight();
void setDirectionLeft();
void setDirectionStraight();
void goStraight(int speed);
void turnAround();
void turnRight();
void turnLeft();
void turnSlightRight();
void turnSlightLeft();
void moveForward(int stepsNeeded);
void moveBackward(int stepsNeeded);

// Transitions between tasks
void moveIntoCanyon(); // this one is maybe not necessary
int senseLineEndOfTask();
void turnRightGetOnLine();
void turnLeftGetOnLine();


// Line Following
void lineNav();
void senseLine();
int checkOffLine();
int checkRight();
int checkLeft();

// Canyon navigation
void canyonNav();
int senseWallRight();
int senseWallLeft();
int Collision();

// Sample collection
void collectSample();

// Sample return
int senseBallWhite();
void depositBlackBall();
void depositWhiteBall();

// Data transmission
void returnHome();
void pointLaser();

#endif	/* ACTIONS_H */
