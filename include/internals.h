#include "karel.h"

#ifndef _INTERNALS_H
#define _INTERNALS_H


#define MULTIPLIER 1000
#define MAX_WIDTH   30
#define MAX_HEIGHT  30

// global variables (application context)
extern int step_delay;
extern struct summary summary;
extern struct world world;
extern struct robot karel;


/**
 * Summary mode metadata
 */
struct summary {
    bool is_active; // flag to check, if summary is active or not
    char* type;     // type of summary (e.g. json)
};


/**
 * Types of blocks
 */
enum block {
    CLEAR = 0,
    WALL = -1
};


/**
 * World definition
 */
struct world {
    int width, height;  // world's width and height
    int **data;         // world data definition
};


/**
 * Available directions
 */
enum direction {
    EAST = 0,
    NORTH = 90,
    WEST = 180,
    SOUTH = 270
};


/**
 * Robot definition
 */
struct robot {
    int x, y;       // position
    enum direction direction;   // direction
    int steps;      // nr. of steps
    int beepers;    // nr. of beepers in bag
    bool is_running; // robot's state
    char *last_command;   // last executed command
};


/**
 * Available colors
 */
enum Colors {
    RED_ON_BLACK = 1,
    YELLOW_ON_BLACK,
    WHITE_ON_BLACK
};


/**
 * Draws scene only
 * Draws only world, without karel and state info
 */
void _draw_world();

/**
 * Render the screen
 * Renders the whole screen - with info panel and world together.
 */
void _render();

void _update(int dx, int dy);

void _error_shut_off(const char *format, ...);


/**
 * initilaize curses, and colors if possible
 */
void _initialize();

void _deinit();

/**
 * Exports data about world and karel
 * Export is useful in summary mode only. Summary mode can be enabled
 * with environment variable LIBKAREL_SUMMARY_MODE with it's value "true".
 * @param struct world world data
 * @param struct robot robot karel
 */
void _export_data(struct world world, struct robot karel);


/**
 * Checks if Karel is turned on and quits program, if not
 * Private function.
 */
void _check_karel_state();


#endif    /* _INTERNALS_H */

