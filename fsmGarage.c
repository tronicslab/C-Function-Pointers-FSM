/******************************************************************************
 * Includes and Defines                                                       *
 ******************************************************************************/

#include <stdio.h> // printf

#define FALSE     0
#define TRUE      1

/******************************************************************************
 * Function Prototypes                                                        *
 ******************************************************************************/

uint8_t btnPress(void);
uint8_t virtualBtnPress(void);

void handle_state_DoorClosed(stateElement *current_state);
void handle_state_DoorOpen(stateElement *current_state);
void handle_state_DoorClosing(stateElement *current_state);
void handle_state_DoorOpening(stateElement *current_state);
void handle_state_DoorStopped(stateElement *current_state);
void handle_state_DoorUnknown(stateElement *current_state);

/******************************************************************************
 * Variables and Type Definitions                                             *
 ******************************************************************************/

typedef void (*fp_doState)(stateElement *x);  /* this defines a type fp_doState */

typedef enum {
    DOOROPENING = 0,        // basically the same state. previous state decides which of
    DOORCLOSING = 0,        // these states becomes the next state after button press event
    DOORSTOPPED = 0,
    DOORCLOSED,
    DOOROPEN,
    DOORUNKNOWN
} state;

typedef struct {
    state lastState;
    state thisState;
    uint8_t currStateConfirmed;
} stateElement;

int main(void) {
    stateElement currentState;
    initializeState(&currentState);
    while(1) {
        fp_doState(&currentState);
        // do other stuff like monitor control from phones
        // update lcd, and time.  
    }
}

/******************************************************************************
 * Functions                                                                  *
 ******************************************************************************/

/****************************************************************
 * State handling functions                                     *
*****************************************************************/

void handle_state_DoorClosed(stateElement *current_state) {
    if( (!current_state->currStateConfirmed) && (checkSensors() == current_state->thisState) ) {
        current_state->currStateConfirmed = TRUE;
    }
    if(btnPress() || virtualBtnPress()) {
        fp_doState = handle_state_DoorOpening;
        current_state->lastState = current_state->thisState;
        current_state->thisState = DOOROPENING; 
        current_state->currStateConfirmed = FALSE;
    }
}

void handle_state_DoorOpen(stateElement *current_state) {
    if( (!current_state->currStateConfirmed) && (checkSensors() == current_state->thisState) ) {
        current_state->currStateConfirmed = TRUE;
    }
    if(btnPress() || virtualBtnPress()) {
        fp_doState = handle_state_DoorClosing;
        current_state->lastState = current_state->thisState;
        current_state->thisState = DOORCLOSING; 
        current_state->currStateConfirmed = FALSE;
    }
}

void handle_state_DoorClosing(stateElement *current_state) { // check for timeout if no concrete state is reached after closing starts 
    if( (!current_state->currStateConfirmed) && (checkSensors() == current_state->thisState) && (current_state->lastState == DOOROPEN) ) {
        current_state->currStateConfirmed = TRUE;
    }
    if( (current_state->currStateConfirmed) && (checkSensors() == DOORCLOSED) ) { // Door has finished closing
        fp_doState = handle_state_DoorClosed;
        current_state->lastState = current_state->thisState;
        current_state->thisState = DOORCLOSED; 
        current_state->currStateConfirmed = TRUE;
    }
    if(btnPress()) {
        fp_doState = handle_state_DoorStopped;
        current_state->lastState = current_state->thisState;
        current_state->thisState = DOORSTOPPED; 
        current_state->currStateConfirmed = FALSE;
    }
    /*if(virtualBtnPress()) {
        // Cannot stop door close remotely
    }*/
}

void handle_state_DoorOpening(stateElement *current_state) { // check for timeout if no concrete state is reached after opening starts
    if( (!current_state->currStateConfirmed) && (checkSensors() == current_state->thisState) ) {
        current_state->currStateConfirmed = TRUE;
    }
    if( (current_state->currStateConfirmed) && (checkSensors() == DOOROPEN) ) { // Door has finished opening
        fp_doState = handle_state_DoorOpen;
        current_state->lastState = current_state->thisState;
        current_state->thisState = DOOROPEN; 
        current_state->currStateConfirmed = TRUE;
    }
    if(btnPress()) {
        fp_doState = handle_state_DoorStopped;
        current_state->lastState = current_state->thisState;
        current_state->thisState = DOORSTOPPED; 
        current_state->currStateConfirmed = FALSE;
    }
    /*if(virtualBtnPress()) {
        // Cannot stop door open remotely
    }*/
}

void handle_state_DoorStopped(stateElement *current_state) {
    if( (!current_state->currStateConfirmed) && (checkSensors() == current_state->thisState) ) {
        current_state->currStateConfirmed = TRUE;
    }
    if(btnPress() || virtualBtnPress()) {
        if(current_state->lastState == DOOROPENING) {
            fp_doState = handle_state_DoorClosing;
            current_state->lastState = current_state->thisState;
            current_state->thisState = DOORCLOSING;
            current_state->currStateConfirmed = FALSE;
        } else if(current_state->lastState == DOORCLOSING) {
            fp_doState = handle_state_DoorOpening;
            current_state->lastState = current_state->thisState;
            current_state->thisState = DOOROPENING;
            current_state->currStateConfirmed = FALSE;
        } else {
            fp_doState = handle_state_DoorUnknown;
            current_state->lastState = current_state->thisState;
            current_state->thisState = DOORUNKNOWN;
            current_state->currStateConfirmed = FALSE;
        }
    }
}

void handle_state_DoorUnknown(stateElement *current_state) { // cannot confirm doorunknown state?
    if(btnPress() || virtualBtnPress()) {
        fp_doState = handle_state_DoorOpening;
    }

}

/****************************************************************
 * Button press functions. We have separate functions for the   *
 * physical buttons and the virtual button attached to the Pi.  *
 * This is to segregate where the action came from and to give  *
 * the ability to disallow the virtual button from stopping a   *
 * currently opening or closing door. 
 * A sensor state of 1, 1 is invalid - door can't be open and   *
 * closed at the same time                                      *
 ****************************************************************/

uint8_t btnPress(void) { 
    // check if button pressed
}

uint8_t virtualBtnPress(void) {
    // check if virtual button pressed
}

/****************************************************************
 * Check the value of our door open/closed sensors. Return 0 if *
 * the sensor is not blocked, and 1 if the sensor is blocked.   *
 * A sensor state of 1, 1 is invalid - door can't be open and   *
 * closed at the same time                                      *
 ****************************************************************/

uint8_t checkSensors(void) {
    // get sensors value
    // sensor open, sensor closed
    // 0, 0 door stopped or opening or closing
    // 0, 1 door closed
    // 1, 0 door open
    // 1, 1 invalid
    uint8_t openSensor = readOpenSensor();
    uint8_t closedSensor = readClosedSensor();
    if(openSensor && !closedSensor) {         // door open
        return (state) DOOROPEN;
    } else if(!openSensor && closedSensor) {  // door closed
        return (state) DOORCLOSED;
    } else if(!openSensor && !closedSensor) { // door opening, closing, or stopped
        return (state) DOORSTOPPED;       
    } else {                      // error - sensors cannot be both on
        return (state) DOORUNKNOWN;
    }
}

/****************************************************************
 * Initialize our entry state by reading the door open/closed   *
 * sensors. If the sensors are in a configuration that has      *
 * multiple states associated then remain in unknown state      *
 * until we reach a known state...most likely by a button press *
 ****************************************************************/

void initializeState(stateElement *current_state) {
    current_state->lastState = DOORUNKNOWN;
    switch(checkSensors()) {
        case DOORCLOSED:
            current_state->thisState = DOORCLOSED;
            current_state->currStateConfirmed = TRUE;
            break;
        case DOOROPEN:
            current_state->thisState = DOOROPEN;
            current_state->currStateConfirmed = TRUE;
            break;
        default:    
            /* No known previous state yet upon power up. Any remaining 
               possible sensor values require a previous state to know
               what state to transition to. No state change until opened 
               or closed state confirmed.                                */
            current_state->thisState = DOORUNKNOWN;
            current_state->currStateConfirmed = FALSE;
            break;

    }
}