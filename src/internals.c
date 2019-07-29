#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <libintl.h>

#define _(STRING) gettext(STRING)

#include "internals.h"


// global variables (application context)
int step_delay = 700 * MULTIPLIER;

struct world world;

struct robot karel; //=  {
//    0,0,     // position
//    EAST,   // direction
//    0,      // steps
//    0,      // beepers in bag
//    false,  // is running
//    "none"  // last command
//};

struct summary summary = {
    false,  // not active
    NULL    // type
};


void _print_beeper(int nr){
    if(summary.is_active == true){
        return;
    }

    if(has_colors()){
        attron(COLOR_PAIR(WHITE_ON_BLACK) | A_BOLD);
    }

    printw("%-2d", nr);

    if(has_colors()){
        attroff(COLOR_PAIR(WHITE_ON_BLACK) | A_BOLD);
    }
}


void _draw_world(){
    if(summary.is_active){
        return;
    }

    // draw horizontal border line
    mvprintw(4, 0, "ST.+");
    for(int column = 0; column <= world.width * 2; column++){
        printw("-");
    }
    printw("+\n");

    // draw world
    for(int row = world.height - 1; row >= 0; row--){
        if(row % 2 == 0){
            printw("%2d |", row / 2 + 1);
        }else{
            printw("   |");
        }

        // small indentation for first column
        if(world.data[row][0] == WALL){
            printw("-");
        }else{
            printw(" ");
        }

        for(int column = 0; column < world.width; column++){
            int block = world.data[row][column];
            int left = column - 1 >= 0 ? world.data[row][column-1] : WALL;
            int right = column + 1 < world.width ? world.data[row][column+1] : WALL;
            int up = row + 1 < world.height ? world.data[row+1][column] : 0;
            int down = row - 1 >= 0 ? world.data[row-1][column] : 0;

            // handle karel positions and beepers
            if(column % 2 == 0 && row % 2 == 0){ // karel can move on even positions
                if(block > 0){
                    _print_beeper(block);
                }else{
                    printw(". ");
                }
                continue;
            }

            // draw walls
            if(block == WALL){
                // vertical wall
                if(column % 2 == 1 && row % 2 == 0){
                    printw("| ");
                    continue;
                }

                if((up == WALL && down == WALL && left != WALL && right != WALL )){
                    printw("| ");
                    continue;
                }

                // horizontal walls
                // strict
                // wall "- "
                if(left == WALL && right != WALL && up != WALL && down != WALL){
                    printw("- ");
                    continue;
                }

                // wall "--"
                if(up != WALL && down != WALL){
                    printw("--");
                    continue;
                }

                // wall " -"
                if(left != WALL && right == WALL && up != WALL && down != WALL){
                    printw(" -");
                    continue;
                }

                // not so strict
                // wall "+-"
                if(right == WALL && (up == WALL || down == WALL)){
                    printw("+-");
                    continue;
                }

                // single wall "| " at top or bottom
                 if(left != WALL && right != WALL &&
                         ((up != WALL && down == WALL) || (down != WALL && up == WALL))){
                    printw("| ");
                    continue;
                }

                // wall "+ " (from left -+)
                if(left == WALL && right != WALL && (up == WALL || down == WALL)){
                    printw("+ ");
                    continue;
                }

                printw("  ");

            }else{
                printw("  ");
            }
        }

        printw("|\n");
    }

    // draw footer
    printw("   +");
    for(int column = 0; column <= world.width * 2; column++){
        printw("-");
    }
    printw("+\n     ");

    for(int column = 0; column < world.width; column++){
        if(column % 2 == 0){
            printw("%-2d", column / 2 + 1);
        }else{
            printw("  ");
        }
    }
    printw("  AVE.\n");

    refresh();
}


void _update(int dx, int dy){
    // update old position, if karel has moved
    if(!(dx == 0 && dy == 0)){
        int block = world.data[karel.y - 2*dy][karel.x - 2*dx];

        if(!summary.is_active){
            move(world.height - (karel.y - 2*dy) + 4, 2 * (karel.x - 2*dx) + 5);
            if(block > 0){
                _print_beeper(block);
            }else{
                printw(". ");
            }
        }
    }
}


void _render(){
    if(summary.is_active == true){
        return;
    }

    // get the string representation of current orientation
    char* direction;
    switch(karel.direction){
        case NORTH  : direction = _("NORTH"); break;
        case SOUTH  : direction = _("SOUTH"); break;
        case WEST   : direction = _("WEST"); break;
        case EAST   : direction = _("EAST"); break;
        default     : direction = _("UNKNOWN"); break;
    }

    // draw header first
    mvprintw(1, 0, " %-3d %s\n", karel.steps, karel.last_command);
    printw(" CORNER  FACING  BEEP-BAG  BEEP-CORNER\n");
    printw(" (%d, %d)   %-5s     %2d        %2d\n",
            (karel.x+2)/2, (karel.y+2)/2, direction, karel.beepers,
            world.data[karel.y][karel.x]);

    // set karel's new position
    move(world.height - karel.y + 4, 2 * karel.x + 5);

    if(has_colors()){
        attron(COLOR_PAIR(YELLOW_ON_BLACK) | A_BOLD);
    }

    // draw karel
    switch(karel.direction){
        case NORTH  : printw("^ "); break;
        case SOUTH  : printw("v "); break;
        case EAST   : printw("> "); break;
        case WEST   : printw("< "); break;
    }

    if(has_colors()){
        attroff(COLOR_PAIR(YELLOW_ON_BLACK) | A_BOLD);
    }

    refresh();
    usleep(get_step_delay());
}


void _error_shut_off(const char* format, ...){
    if(!summary.is_active){
        if(has_colors()){
            attron(COLOR_PAIR(RED_ON_BLACK));
        }

        mvprintw(0, 0, _("Error Shutoff! (%s)"), format);

        refresh();
        getchar();
        endwin();
    }else{
        fprintf(stderr, _("Error Shutoff! (%s)"), format);
    }

    exit(EXIT_FAILURE);
}


void _initialize(){
    if(summary.is_active){
        return;
    }

    // handle CTRL+C (signal interrupt)
    signal(SIGINT, turn_off);

    // init ncurses
    initscr();
    if(has_colors()){
        start_color();
        init_pair(RED_ON_BLACK, COLOR_RED, COLOR_BLACK); // RED on BLACK
        init_pair(YELLOW_ON_BLACK, COLOR_YELLOW, COLOR_BLACK); // YELLOW on BLACK
        init_pair(WHITE_ON_BLACK, COLOR_WHITE, COLOR_BLACK); // WHITE on BLACK
    }

    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    cbreak();

    // hide cursor
    curs_set(0);
}


void _deinit(){
    if(summary.is_active){
        return;
    }

    if(has_colors()){
        attron(COLOR_PAIR(YELLOW_ON_BLACK));
    }
    mvprintw(0, 0, _("Press any key to quit..."));
    refresh();
    getchar();
    endwin();
}


void _export_data(){
    // prepare direction
    char direction;
    switch(karel.direction){
        case EAST   : direction = 'E'; break;
        case NORTH  : direction = 'N'; break;
        case WEST   : direction = 'W'; break;
        case SOUTH  : direction = 'S'; break;
        default     : direction = ' '; break;
    }

    if(summary.type && strcmp(summary.type, "json") == 0){
        printf("{\n"
                "\"x\": %d,\n"
                "\"y\": %d,\n"
                "\"direction\": \"%c\",\n"
                "\"bag\": %d,\n"
                "\"beepers\": [\n",
                (karel.x+2)/2, (karel.y+2)/2, direction, karel.beepers);

        // export world
        bool any_beeper = false;
        for(size_t row = 0; row < world.height/2+1; row++){
            for(size_t col = 0; col < world.width/2+1; col++){
                if(world.data[row][col] > 0){
                    if(any_beeper == false){
                        any_beeper = true;
                    }else{
                        printf(",\n");
                    }
                    printf("    {\"x\": %zu, \"y\": %zu, \"count\": %d}", col, row, world.data[row][col]);
                }
            }
        }

        printf("\n]}");
    }else{
        // header
        printf("%d %d %c %d\n",
                (karel.x+2)/2, (karel.y+2)/2, direction, karel.beepers);

        // export world
        for(size_t row = 0; row < world.height/2+1; row++){
            for(size_t col = 0; col < world.width/2+1; col++){
                if(world.data[row][col] > 0){
                    printf("B %zu %zu %d\n", col, row, world.data[row][col]);
                }
            }
        }
    }
}


void _check_karel_state(){
     if(!karel.is_running){
        _error_shut_off(_("Karel is not turned On"));
     }
}
