#ifndef _KAREL_H
#define	_KAREL_H

#include <stdio.h>
#include <stdbool.h>


// loop() macro to replace for()
#define loop(COUNT) for(size_t __loop_cntr = 0; __loop_cntr < COUNT; __loop_cntr++)


// *************************************** Primitives 

/**
 * Moves Karel one step forward
 * If there is wall or Karel is at the border of the world, he will be turned 
 * off automatically.
 */
void movek();


/**
 * Turns Karel 90 degrees left
 */
void turnLeft();


/**
 * Initializes the world of Karel the Robot
 * Function has one parameter, which defines the location of world file. If
 * the file doesn't exist, program will be terminated with error message.
 * @param path location of the world file
 */
void turnOn(char* path);


/**
 * Terminates  Karel's program
 */
void turnOff();


/**
 * Puts beeper at the current world position, if Karol has some
 */
void putBeeper();


/**
 * Picks beeper from current position if there is any and puts it to Karol's bag
 */
void pickBeeper();


// *************************************** Sensors

/**
 * Checks, if there are beepers present at the corner
 * @return true, if there are beepers, false otherwise
 */
bool beepersPresent();


/**
 * Checks, if there are any beepers in the bag
 * @return true, if there are some, false otherwise
 */
bool beepersInBag();


/**
 * Checks, if front of Karel is clear to go
 * @return true, if clear, false otherwise
 */
bool frontIsClear();


/**
 * Checks, if Karel is facing north
 * @return true, if yes, false otherwise
 */
bool facingNorth();


// *************************************** Functions

/**
 * Sets the delay of one step
 * @param delay the delay in millis
 */
void setStepDelay(int delay);


/**
 * Returns the current value of Karel's step delay
 * @return the step delay value in millis
 */
int getStepDelay();

#endif	/* _KAREL_H */
