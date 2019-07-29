#include <stdbool.h>

#include <libintl.h>

#define _(STRING) gettext(STRING)

#include "superkarel.h"
#include "internals.h"


bool no_beepers_in_bag() {
    return !beepers_in_bag();
}


bool front_is_blocked() {
    return !front_is_clear();
}


bool left_is_clear() {
    _check_karel_state();

    enum direction originalDirection = karel.direction;
    karel.direction += 90;
    if ((int) karel.direction > 270) {
        karel.direction = EAST;
    }

    bool isClear = front_is_clear();
    karel.direction = originalDirection;

    return isClear;
}


bool left_is_blocked() {
    return !left_is_clear();
}


bool right_is_clear() {
    _check_karel_state();

    enum direction originalDirection = karel.direction;
    karel.direction -= 90;
    if ((int) karel.direction < 0) {
        karel.direction = SOUTH;
    }

    bool isClear = front_is_clear();
    karel.direction = originalDirection;

    return isClear;
}


bool right_is_blocked() {
    return !right_is_clear();
}


bool not_facing_north() {
    return !facing_north();
}


bool facing_south() {
    _check_karel_state();
    return karel.direction == SOUTH;
}


bool not_facing_south() {
    return !facing_south();
}


bool facing_east() {
    _check_karel_state();
    return karel.direction == EAST;
}


bool not_facing_east() {
    return !facing_east();
}


bool facing_west() {
    _check_karel_state();
    return karel.direction == WEST;
}


bool not_facing_west() {
    return !facing_west();
}


bool no_beepers_present() {
    return !beepers_present();
}
