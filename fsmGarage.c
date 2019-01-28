// NOTE = checksensors thinks enum opening closing stopped are the same
/******************************************************************************
 * Includes and Defines                                                       *
 ******************************************************************************/

#include <stdio.h>    // printf etc..
#include <inttypes.h> // uint8_t etc..

#define FALSE     0
#define TRUE      1

/******************************************************************************
 * Variables and Type Definitions                                             *
 ******************************************************************************/

typedef enum State {
    DOOROPENING,        // basically the same state. previous state decides which of
    DOORCLOSING,        // these states becomes the next state after button press event
    DOORSTOPPED,
    DOORCLOSED,
    DOOROPEN,
    DOORUNKNOWN
} State;

typedef void (*fp_doState) (void*);  /* this defines a type fp_doState */

typedef struct stateElement{
    fp_doState do_state;
    State lastState;
    State thisState;
    uint8_t currStateConfirmed;
}stateElement;

/******************************************************************************
 * Functions                                                                  *
 ******************************************************************************/

/****************************************************************
 * Function Prototypes                                          *
 ****************************************************************/

uint8_t btnPress(void);
uint8_t virtualBtnPress(void);

State checkSensors(stateElement *current_state);
uint8_t readClosedSensor(void);
uint8_t readOpenSensor(void);

void initializeState(stateElement *current_state);

void handle_state_DoorClosed(stateElement *current_state);
void handle_state_DoorOpen(stateElement *current_state);
void handle_state_DoorClosing(stateElement *current_state);
void handle_state_DoorOpening(stateElement *current_state);
void handle_state_DoorStopped(stateElement *current_state);
void handle_state_DoorUnknown(stateElement *current_state);

/****************************************************************
 * Main Program                                                 *
*****************************************************************/
uint8_t fakeButton = 0;
uint8_t fakevButton = 0;
uint8_t openSense = 0;
uint8_t closedSense = 1;


int main(void) {
    int loopcount = 0;    
    printf("\n\nLoop %d\n", loopcount);
    stateElement currentState;
    printf("Getting initial state...\n");
    initializeState(&currentState);
    printf("lastState %d\nthisState %d\n", currentState.lastState, currentState.thisState);
    while(loopcount < 24) {
        printf("\n\nLoop %d\n", loopcount);
        printf("lastState %d\nthisState %d\n", currentState.lastState, currentState.thisState);
        currentState.do_state(&currentState);
        if(fakeButton || fakevButton) {
            fakeButton = 0;
            fakevButton = 0;
        }
        // do other stuff like monitor control from phones
        // update lcd, and time.  
        loopcount++;
        if(loopcount == 3) {
            fakeButton = 1;
        }
        if(loopcount == 4) {
            closedSense = 0;
        }
        if(loopcount == 6) {
            openSense = 1;
        }
        if(loopcount == 8) {
            fakevButton = 1;
        }
        if(loopcount == 10) {
            openSense = 0;
        }
        if(loopcount == 12) {
            fakeButton = 1;
        }
        if(loopcount == 14) {
            fakeButton = 1;
        }
        if(loopcount == 16) {
            openSense = 1;
        }
        if(loopcount == 18) {
            fakevButton = 1;
        }
        if(loopcount == 20) {
            openSense = 0;
        }
        if(loopcount == 22) {
            closedSense = 1;
        }
    }
}

/****************************************************************
 * State handling functions                                     *
*****************************************************************/

void handle_state_DoorClosed(stateElement *current_state) {
    printf("Handling door closed state\n");
    if( (!current_state->currStateConfirmed) && (checkSensors(current_state) == current_state->thisState) ) {
        printf("Door closed state confirmed!\n");
        current_state->currStateConfirmed = TRUE;
    }
    if(btnPress() || virtualBtnPress()) {
        current_state->do_state = (fp_doState)handle_state_DoorOpening;
        current_state->lastState = current_state->thisState;
        current_state->thisState = DOOROPENING; 
        current_state->currStateConfirmed = FALSE;
    }
}

void handle_state_DoorOpen(stateElement *current_state) {
    printf("Handling door open state\n");
    if( (!current_state->currStateConfirmed) && (checkSensors(current_state) == current_state->thisState) ) {
        printf("Door open state confirmed!\n");
        current_state->currStateConfirmed = TRUE;
    }
    if(btnPress() || virtualBtnPress()) {
        current_state->do_state = (fp_doState)handle_state_DoorClosing;
        current_state->lastState = current_state->thisState;
        current_state->thisState = DOORCLOSING; 
        current_state->currStateConfirmed = FALSE;
    }
}

void handle_state_DoorClosing(stateElement *current_state) { // check for timeout if no concrete state is reached after closing starts 
    printf("Handling door closing state\n");
    if( (!current_state->currStateConfirmed) && (checkSensors(current_state) == current_state->thisState) && (current_state->lastState == DOOROPEN) ) {
        printf("Door closing state confirmed!\n");
        current_state->currStateConfirmed = TRUE;
    }
    if( (current_state->currStateConfirmed) && (checkSensors(current_state) == DOORCLOSED) ) { // Door has finished closing
        current_state->do_state = (fp_doState)handle_state_DoorClosed;
        current_state->lastState = current_state->thisState;
        current_state->thisState = DOORCLOSED;
        printf("Door closed state confirmed!\n");
        current_state->currStateConfirmed = TRUE;
    }
    if(btnPress()) {
        current_state->do_state = (fp_doState)handle_state_DoorStopped;
        current_state->lastState = current_state->thisState;
        current_state->thisState = DOORSTOPPED; 
        current_state->currStateConfirmed = FALSE;
    }
    /*if(virtualBtnPress()) {
        // Cannot stop door close remotely
    }*/
}

void handle_state_DoorOpening(stateElement *current_state) { // check for timeout if no concrete state is reached after opening starts
    printf("Handling door opening state\n");
    if( (!current_state->currStateConfirmed) && (checkSensors(current_state) == current_state->thisState) ) {
        printf("Door opening state confirmed!\n");
        current_state->currStateConfirmed = TRUE;
    }
    if( (current_state->currStateConfirmed) && (checkSensors(current_state) == DOOROPEN) ) { // Door has finished opening
        current_state->do_state = (fp_doState)handle_state_DoorOpen;
        current_state->lastState = current_state->thisState;
        current_state->thisState = DOOROPEN; 
        printf("Door open state confirmed!\n");
        current_state->currStateConfirmed = TRUE;
    }
    if(btnPress()) {
        current_state->do_state = (fp_doState)handle_state_DoorStopped;
        current_state->lastState = current_state->thisState;
        current_state->thisState = DOORSTOPPED; 
        current_state->currStateConfirmed = FALSE;
    }
    /*if(virtualBtnPress()) {
        // Cannot stop door open remotely
    }*/
}

void handle_state_DoorStopped(stateElement *current_state) {
    printf("Handling door stopped state\n");
    if( (!current_state->currStateConfirmed) && (checkSensors(current_state) == current_state->thisState) ) {
        printf("Door stopped state confirmed!\n");
        current_state->currStateConfirmed = TRUE;
    }
    if(btnPress() || virtualBtnPress()) {
        if(current_state->lastState == DOOROPENING) {
            current_state->do_state = (fp_doState)handle_state_DoorClosing;
            current_state->lastState = current_state->thisState;
            current_state->thisState = DOORCLOSING;
            current_state->currStateConfirmed = FALSE;
        } else if(current_state->lastState == DOORCLOSING) {
            current_state->do_state = (fp_doState)handle_state_DoorOpening;
            current_state->lastState = current_state->thisState;
            current_state->thisState = DOOROPENING;
            current_state->currStateConfirmed = FALSE;
        } else {
            current_state->do_state = (fp_doState)handle_state_DoorUnknown;
            current_state->lastState = current_state->thisState;
            current_state->thisState = DOORUNKNOWN;
            current_state->currStateConfirmed = FALSE;
        }
    }
}

void handle_state_DoorUnknown(stateElement *current_state) { // cannot confirm doorunknown state?
    printf("Handling door unknown state\n");
    if(btnPress() || virtualBtnPress()) {
        current_state->do_state = (fp_doState)handle_state_DoorOpening;
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
    printf("Checking for hard button press: ");
    if(fakeButton) {
        printf("Yes\n");
    } else {
        printf("No\n");
    }
    return fakeButton;
}

uint8_t virtualBtnPress(void) {
    // check if virtual button pressed
    printf("Checking for virtual button press: ");
    if(fakevButton) {
        printf("Yes\n");
    } else {
        printf("No\n");
    }
    return fakevButton;
}

/****************************************************************
 * Check the value of our door open/closed sensors. Return 0 if *
 * the sensor is not blocked, and 1 if the sensor is blocked.   *
 * A sensor state of 1, 1 is invalid - door can't be open and   *
 * closed at the same time                                      *
 ****************************************************************/

State checkSensors(stateElement *current_state) {
    // get sensors value
    // sensor open, sensor closed
    // 0, 0 door stopped or opening or closing
    // 0, 1 door closed
    // 1, 0 door open
    // 1, 1 invalid
    printf("Checking sensors...\n");    
    uint8_t openSensor = readOpenSensor();
    uint8_t closedSensor = readClosedSensor();
    if(openSensor && !closedSensor) {         // door open
        return DOOROPEN;
    } else if(!openSensor && closedSensor) {  // door closed
        return DOORCLOSED;
    } else if(!openSensor && !closedSensor) { // door opening, closing, or stopped
        return current_state->thisState;
    } else {                                  // error - sensors cannot be both on
        printf("ERROR: Sensor indicates both open and closed!\n");        
        return DOORUNKNOWN;
    }
}

uint8_t readOpenSensor(void) {
    // read sensor, return 1 for sensor blocked, 0 for sensor open
    printf("Reading Open sensor: ");
    if(openSense) {
        printf("Yes\n");
    } else {
        printf("No\n");
    }
    return openSense;
}

uint8_t readClosedSensor(void) {
    // read sensor, return 1 for sensor blocked, 0 for sensor open
    printf("Reading Closed sensor: ");
    if(closedSense) {
        printf("Yes\n");
    } else {
        printf("No\n");
    }
    return closedSense;
}

/****************************************************************
 * Initialize our entry state by reading the door open/closed   *
 * sensors. If the sensors are in a configuration that has      *
 * multiple states associated then remain in unknown state      *
 * until we reach a known state...most likely by a button press *
 ****************************************************************/

void initializeState(stateElement *current_state) {
    current_state->lastState = DOORUNKNOWN;
    switch(checkSensors(current_state)) {
        case DOORCLOSED:
            printf("Sensors indicate door closed, setting state DOORCLOSED, state confirmed!\n");
            current_state->thisState = DOORCLOSED;
            current_state->currStateConfirmed = TRUE;
            current_state->do_state = (fp_doState)handle_state_DoorClosed;
            break;
        case DOOROPEN:
            printf("Sensors indicate door open, setting state DOOROPEN, state confirmed!\n");
            current_state->thisState = DOOROPEN;
            current_state->currStateConfirmed = TRUE;
            current_state->do_state = (fp_doState)handle_state_DoorOpen;
            break;
        default:    
            /* No known previous state yet upon power up. Any remaining 
               possible sensor values require a previous state to know
               what state to transition to. No state change until opened 
               or closed state confirmed.                                */
            printf("Sensors indicate unknown door position, setting state DOORUNKNOWN, state not confirmed!\n");
            current_state->thisState = DOORUNKNOWN;
            current_state->currStateConfirmed = FALSE;
            current_state->do_state = (fp_doState)handle_state_DoorUnknown;
            break;

    }
}
