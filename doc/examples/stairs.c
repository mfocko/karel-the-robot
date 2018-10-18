#include <karel.h>

void turnRight(){
    loop(3){
        turnLeft();
    }
}

int main(){
    // turn on karel in the world stairs.kw
    turnOn("stairs.kw");
    
    // pick all beepers on all stairs
    loop(3){
        turnLeft();
        movek();
        turnRight();
        movek();
        pickBeeper();
    }

    // climb the top stair
    turnLeft();
    movek();
    turnRight();
    movek();

    // put all beepers
    while(beepersInBag()){
        putBeeper();
    }

    // turn off
    turnOff();
}
