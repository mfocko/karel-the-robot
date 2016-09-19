#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <string.h>

#include <libintl.h>
#include <locale.h>

#define _(STRING) gettext(STRING)


#include "internals.h"
#include "karel.h"

// global variables (application context)
int stepDelay = 700 * MULTIPLIER;
bool summary_mode = false;

struct world world;
struct robot karel =  {
        0,0,    // position
        EAST,   // direction
        0,      // steps
        0,      // beepers in bag
        false,  // is running
        "none"  // last command
};


void printBeeper(int nr){
    if(summary_mode == true){
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


void drawWorld(struct world world, struct robot karel){
    if(summary_mode == true){
        return;
    }

    int column, row;

    move(4, 0);
    // draw horizontal border line
    printw("ST.+");
    for(column = 0; column <= world.width * 2; column++){
        printw("-");
    }
    printw("+\n");

    // draw world
    for(row = world.height - 1; row >= 0; row--){
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

        for(column = 0; column < world.width; column++){
            int block = world.data[row][column];
            int left = column - 1 >= 0 ? world.data[row][column-1] : WALL;
            int right = column + 1 < world.width ? world.data[row][column+1] : WALL;
            int up = row + 1 < world.height ? world.data[row+1][column] : 0;
            int down = row - 1 >= 0 ? world.data[row-1][column] : 0;

            // handle karel possitions and beepers
            if(column % 2 == 0 && row % 2 == 0){ // karel can move on even positions
                if(block > 0){
                    printBeeper(block);
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

                if((up == WALL && down == WALL && left != WALL && right != WALL )){// ||
                        //(up == WALL && (down != WALL && left != WALL && right != WALL)) ||
                       // (down == WALL && (up != WALL && left != WALL && right != WALL)) ){
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
                if(up != WALL && down != WALL){//&& left == WALL && right == WALL ){
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
                if(right == WALL && ( (up == WALL || down == WALL)
                            || (up == WALL && left == WALL)
                            || (up == WALL && down == WALL) 
                            || (left == WALL && down == WALL) )){
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
    for(column = 0; column <= world.width * 2; column++){
        printw("-");
    }
    printw("+\n     ");

    for(column = 0; column < world.width; column++){
        if(column % 2 == 0){
            printw("%-2d", column / 2 + 1);
        }else{
            printw("  ");
        }
    }
    printw("  AVE.\n");

    refresh();
}


void update(struct world world, struct robot karel, int dx, int dy){
    // update old position, if karel has moved
    if(!(dx == 0 && dy == 0)){
        int block = world.data[karel.y - 2*dy][karel.x - 2*dx];

        if(!summary_mode){
            move(world.height - (karel.y - 2*dy) + 4, 2 * (karel.x - 2*dx) + 5);
            if(block > 0){
                printBeeper(block);
            }else{
                printw(". ");
            }
        }
    }
}


void render(struct world world, struct robot karel){
    if(summary_mode == true){
        return;
    }

    char direction[10];
    move(1,0);

    // get the string representation of current orientation
    switch(karel.direction){
        case NORTH  : sprintf(direction, _("NORTH")); break;
        case SOUTH  : sprintf(direction, _("SOUTH")); break;
        case WEST   : sprintf(direction, _("WEST")); break;
        case EAST   : sprintf(direction, _("EAST")); break;
        default     : sprintf(direction, _("UNKNOWN")); break;
    }

    // draw header first
    printw(" %-3d %s\n", karel.steps, karel.lastCommand);
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
    usleep(getStepDelay());
}


void errorShutOff(char* message){
    if(summary_mode != true){
        move(0, 0);
        if(has_colors()){
            attron(COLOR_PAIR(RED_ON_BLACK));
        }
        printw(_("Error Shutoff! (%s)"), message);
        refresh();
        getchar();
        endwin();
    }else{
        fprintf(stderr, _("Error Shutoff! (%s)"), message);
    }

    exit(EXIT_FAILURE);
}


/**
 * initilaize curses, and colors if possible
 */
void initialize(){
    if(summary_mode == true){
        return;
    }

    initscr();
    if(has_colors()){
        start_color();
        init_pair(RED_ON_BLACK, COLOR_RED, COLOR_BLACK); // RED on BLACK
        init_pair(YELLOW_ON_BLACK, COLOR_YELLOW, COLOR_BLACK); // YELLOW on BLACK
        init_pair(WHITE_ON_BLACK, COLOR_WHITE, COLOR_BLACK); // WHITE on BLACK
    }

    // hide cursor
    curs_set(0);
}


void deinit(){
    if(summary_mode){
        return;
    }

    move(0, 0);
    if(has_colors()){
        attron(COLOR_PAIR(YELLOW_ON_BLACK));
    }
    printw(_("Press any key to quit..."));
    refresh();
    getchar();
    endwin();
}

void export_data(struct world world, struct robot karel){
    // prepare direction
    char direction = ' ';
    switch(karel.direction){
        case 0      : direction = 'E'; break;
        case 90     : direction = 'N'; break;
        case 180    : direction = 'W'; break;
        case 270    : direction = 'S'; break;
    }

    char* type = getenv("LIBKAREL_SUMMARY_MODE_TYPE");
    if(type && strcmp(type, "json") == 0){
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
                    printf("    {\"x\": %ld, \"y\": %ld, \"count\": %d}", col, row, world.data[row][col]);
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
                    printf("B %ld %ld %d\n", col, row, world.data[row][col]);
                }
            }
        }
    }
}
