#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include <libintl.h>
#include <locale.h>

#define _(STRING) gettext(STRING)

#include "karel.h"
#include "internals.h"



/**
 * Checks if Karel is turned on and quits program, if not
 * Private function.
 */
void checkKarelState(){
     if(!karel.isRunning){
        errorShutOff(_("Karel is not turned On"));
     }
}


// *************************************** Sensors

bool beepersInBag(){
    checkKarelState();
    return karel.beepers > 0;
}


bool noBeepersInBag(){
    return !beepersInBag();
}


bool frontIsClear(){
    checkKarelState();

    switch(karel.direction){
        case NORTH    :
            if(karel.y+1 >= world.height || world.data[karel.y+1][karel.x] == WALL){
                return false;
            }
            break;
        case SOUTH    :
            if(karel.y-1 < 1 || world.data[karel.y-1][karel.x] == WALL){
                return false;
            }
            break;
        case WEST    :
            if(karel.x-1 < 1 || world.data[karel.y][karel.x-1] == WALL){
                return false;
            }
            break;
        case EAST    :
            if(karel.x+1 >= world.width || world.data[karel.y][karel.x+1] == WALL){
                return false;
            }
            break;
    }

    return true;
}


bool frontIsBlocked(){
    return !frontIsClear();
}


bool leftIsClear(){
    checkKarelState();

    enum direction originalDirection = karel.direction;
    karel.direction += 90;
    if((int)karel.direction > 270){
        karel.direction = EAST;
    }

    bool isClear = frontIsClear();
    karel.direction = originalDirection;

    return isClear;
}


bool leftIsBlocked(){
    return !leftIsClear();
}


bool rightIsClear(){
    checkKarelState();

    enum direction originalDirection = karel.direction;
    karel.direction -= 90;
    if((int)karel.direction < 0){
        karel.direction = SOUTH;
    }

    bool isClear = frontIsClear();
    karel.direction = originalDirection;

    return isClear;
}


bool rightIsBlocked(){
    return !rightIsClear();
}


bool facingNorth(){
    checkKarelState();
    return karel.direction == NORTH;
}


bool notFacingNorth(){
    return !facingNorth();
}


bool facingSouth(){
    checkKarelState();
    return karel.direction == SOUTH;
}


bool notFacingSouth(){
    return !facingSouth();
}


bool facingEast(){
    checkKarelState();
    return karel.direction == EAST;
}


bool notFacingEast(){
    return !facingEast();
}


bool facingWest(){
    checkKarelState();
    return karel.direction == WEST;
}


bool notFacingWest(){
    return !facingWest();
}


bool beepersPresent(){
    checkKarelState();
    return world.data[karel.y][karel.x] > 0;
}


bool noBeepersPresent(){
    return !beepersPresent();
}


// *************************************** Primitives 

void movek(){
    checkKarelState();

    if(frontIsClear()){
        switch(karel.direction){
            case NORTH  : karel.y+=2; update(world, karel, 0, 1); break;
            case SOUTH  : karel.y-=2; update(world, karel, 0, -1); break;
            case WEST   : karel.x-=2; update(world, karel, -1, 0); break;
            case EAST   : karel.x+=2; update(world, karel, 1, 0); break;
        }

        karel.steps++;
        karel.lastCommand = _("MOVE");
        render(world, karel);
    }else
        errorShutOff(_("Can't move this way"));
}


void turnLeft(){
    checkKarelState();

    karel.direction += 90;
    if(karel.direction > 270){
        karel.direction = EAST;
    }
    karel.steps++;
    karel.lastCommand = _("TURN LEFT");
    
    update(world,karel,0,0);
    render(world, karel);
}


void turnOff(){
    karel.lastCommand = _("TURN OFF");
    render(world, karel);
    deinit();

    // if summary, then export data
    if(summary_mode){
        export_data(world, karel);
    }
    
    // free memory
    for(size_t y = 0; y < world.height; y++){
        free(world.data[y]);
    }
    free(world.data);

    // credits
    if(!summary_mode){
        printf("Created by miroslav.binas@tuke.sk (c)2010, 2016, 2018\n");
    }

    exit(EXIT_SUCCESS);
}


void turnOn(char* path){
    int row, column, count;
    char direction, block, orientation;

    // open file and read the world params
    FILE *fp = fopen(path, "r");
    if(fp == NULL){
        fprintf(stderr, _("Error: World file '%s' not found.\n"), path);
        exit(EXIT_FAILURE);
    }

    if(fscanf(fp, "%d %d %d %d %c %d\n",
            &world.width, &world.height, &karel.x, &karel.y, &direction,
            &karel.beepers) != 6){
        fprintf(stderr, _("Error: The world informations are not in correct format!\n"));
        exit(EXIT_FAILURE);
    }

    // world correction
    world.width = world.width*2-1;
    world.height = world.height*2-1;

    // karel position correction
    karel.x = karel.x * 2 - 2;
    karel.y = karel.y * 2 - 2;

    // check the world dimensions
    if(world.width > MAX_WIDTH || world.width < 1){
        fprintf(stderr, 
                _("Error: The world's width is outside the range [1, %d]!\n"), 
                MAX_WIDTH);
        exit(EXIT_FAILURE);
    }
    
    if(world.height > MAX_HEIGHT || world.height < 1){
        fprintf(stderr, 
                _("Error: The world's height is outside the range [1, %d]!\n"), 
                MAX_HEIGHT);
        exit(EXIT_FAILURE);
    }

    // format the direction
    switch(toupper(direction)){
        case 'S'    : karel.direction = SOUTH; break;
        case 'W'    : karel.direction = WEST; break;
        case 'E'    : karel.direction = EAST; break;
        case 'N'    : karel.direction = NORTH; break;
        default     : fprintf(stderr, _("Error: Uknown Karel's direction\n"));
                      exit(EXIT_FAILURE);
    }
    
    // create empty world
    world.data = (int**)calloc(world.height, sizeof(int*));
    for(size_t y = 0; y < world.height; y++){
        world.data[y] = (int*)calloc(world.width, sizeof(int));
    }

    // load the map description
    int line = 2;
    while(fscanf(fp, "%c", &block) != EOF){

        switch(toupper(block)){
            case 'W':   {
                int column2, row2;

                // read the data
                if(fscanf(fp, "%d %d %c\n", &column, &row, &orientation) != 3){
                    fprintf(stderr, _("Error: Line %d: Incorrect format.\n"), line);
                    exit(EXIT_FAILURE);
                }

                // check correct position
                if(column > world.width || row > world.height){
                    fprintf(stderr, 
                            _("Error: Line %d: Wall position is outside the world!\n"), 
                            line);
                    exit(EXIT_FAILURE);
                }

                column2 = column*2-2;
                row2 = row*2-2;

                // check, if the wall is correctly positioned
                if(column2 % 2 == 1 || row2 % 2 == 1){
                    fprintf(stderr, _("Error: Wrong position.\n"));
                    exit(EXIT_FAILURE);
                }

                // set walls
                switch(toupper(orientation)){
                    case 'E':   column2++; break;
                    case 'W':   column2--; break;
                    case 'N':   row2++; break;
                    case 'S':   row2--; break;
                    default :   fprintf(stderr, 
                                        _("Error: Uknown wall orientation '%c'"
                                            " on line %d in world file.\n"), 
                                        orientation, line);
                                exit(EXIT_FAILURE);
                }

                world.data[row2][column2] = WALL;

                // set vertical line
                if(column2 % 2 == 1 && row2 % 2 == 0){
                    if(row2 + 1 < world.height){
                        world.data[row2+1][column2] = WALL;
                    }
                    if(row2 - 1 >= 0){
                        world.data[row2-1][column2] = WALL;
                    }
                }else{
                    // set horizontal line
                    if(column2 + 1 < world.width){
                        world.data[row2][column2+1] = WALL;
                    }
                    if(column2 - 1 >= 0){
                        world.data[row2][column2-1] = WALL;
                    }
                }
                break;
            }

            case 'B':   {
                if(fscanf(fp, "%d %d %d\n", &column, &row, &count) != 3){
                    fprintf(stderr, _("Error: Line %d: Incorrect format.\n"), line);
                    exit(EXIT_FAILURE);
                }

                // check correct position
                if(column > world.width || row > world.height){
                    fprintf(stderr, 
                            _("Error: Line %d: Beeper position is outside the world!\n"), 
                            line);
                    exit(EXIT_FAILURE);
                }

                world.data[row*2-2][column*2-2] = count;
                break;
            }

            default :   fprintf(stderr, _("Error: Unknown block character %c "
                                "on line %d in world file.\n"), block, line);
                        exit(EXIT_FAILURE);
        }

        line++;
    }

    // close the file and draw the scene
    fclose(fp);

    // check, if we are running summary mode
    char* mode = getenv("LIBKAREL_SUMMARY_MODE");
    if(mode && strcmp(mode, "true") == 0){
        summary_mode = true;
    }

    // final initialization
    initialize();
    karel.lastCommand = _("TURN ON");
    drawWorld(world, karel);
    render(world, karel);

    karel.isRunning = true;
}


void putBeeper(){
    checkKarelState();

    if(karel.beepers > 0){
        world.data[karel.y][karel.x]++;
        karel.beepers--;
        karel.steps++;
        karel.lastCommand = _("PUT BEEPER");
        render(world, karel);
    }else{
        errorShutOff(_("Karel has no beeper to put at the corner"));
    }
}


void pickBeeper(){
    checkKarelState();

    if(world.data[karel.y][karel.x] > 0){
        world.data[karel.y][karel.x]--;
        karel.beepers++;
        karel.steps++;
        karel.lastCommand = _("PICK BEEPER");
        render(world, karel);
    }else{
        errorShutOff(_("There is no beeper at the corner"));
    }
}

// *************************************** Functions



void setStepDelay(int delay){
    stepDelay = delay * MULTIPLIER;
}


int getStepDelay(){
    return stepDelay;
}
