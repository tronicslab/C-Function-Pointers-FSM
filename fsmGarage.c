#include <stdio.h> // printf

#define FALSE     0
#define TRUE      1

typedef void (*fp_doState)(stateElement *x);  /* this defines a type fp_doState */

uint8_t btnPress(void);
uint8_t virtualBtnPress(void);

void handle_state_DoorClosed(stateElement *current_state);
void handle_state_DoorOpen(stateElement *current_state);
void handle_state_DoorClosing(stateElement *current_state);
void handle_state_DoorOpening(stateElement *current_state);
void handle_state_DoorStopped(stateElement *current_state);
void handle_state_DoorUnknown(stateElement *current_state);

typedef enum {
    DOORCLOSED,
    DOOROPENING,
    DOOROPEN,
    DOORCLOSING,
    DOORSTOPPED,
    DOORUNKNOWN
} state;

typedef struct {
    state lastState;
    state thisState;
    uint8_t currStateConfirmed;
} stateElement;

int main(void) {
    stateElement currentState = { DOORUNKNOWN, DOORUNKNOWN, FALSE};
    checkSensors();
}

void handle_state_DoorClosed(stateElement *current_state) {
    if(btnPress() || virtualBtnPress()) {
        fp_doState = handle_state_DoorOpening;
    }
}

void handle_state_DoorOpen(stateElement *current_state) {
    if(btnPress() || virtualBtnPress()) {
        fp_doState = handle_state_DoorClosing;
    }
}

void handle_state_DoorClosing(stateElement *current_state) {
    if(btnPress()) {
        fp_doState = handle_state_DoorClosed;
    }
    if(virtualBtnPress()) {
        // Cannot stop door close remotely
    }
}

void handle_state_DoorOpening(stateElement *current_state) {
    if(btnPress()) {
        fp_doState = handle_state_DoorOpen;
    }
    if(virtualBtnPress()) {
        // Cannot stop door close remotely
    }
}

void handle_state_DoorStopped(stateElement *current_state) {
    if(btnPress() || virtualBtnPress()) {
        fp_doState = handle_state_DoorUnknown;
    }
}

void handle_state_DoorUnknown(stateElement *current_state) {
    if(btnPress() || virtualBtnPress()) {
        fp_doState = handle_state_DoorOpening;
    }

}

uint8_t btnPress(void) { 
    // check if button pressed
}

uint8_t virtualBtnPress(void) {
    // check if virtual button pressed
}

uint8_t checkSensors(void) {
    // get sensors value
    // sensor open, sensor closed
    // 0, 0 door stopped or opening or closing
    // 0, 1 door closed
    // 1, 0 door open
    // 1, 1 invalid
    uint8_t openSensor = readOpenSensor();
    uint8_t closedSensor = readClosedSensor();
    if(openSensor && !closedSensor) {        // door open
        return (state) DOOROPEN;
    } else if(!openSensor && closedSensor) { // door closed
        return (state) DOORCLOSED;
    } else if(!openSensor && !closedSensor) {
        
    }
}
