#include <stdio.h>

#define CLOSED  0
#define OPEN    1
#define CLOSING 2
#define OPENING 3
#define STOPPED 4
#define UNKNOWN 5
#define OFF     0
#define ON      1

typedef struct {
    int openSW;
    int closedSW;
} Sensors;

typedef void (*fp_event)(int *x);  /* this defines a type fp_event */
int state = CLOSED;

int confirmSensors(int expectedOpenSW, int expectedClosedSW, Sensors *myDoor);
void btnPress(int *current_state);
void virtualBtnPress(int *current_state);

fp_event fp_arr[] = {btnPress, virtualBtnPress};


int main(void) {
    fp_event event_list;
    Sensors doorSensors;

    doorSensors.openSW = ON;
    doorSensors.closedSW = ON;


    event_list = fp_arr[1];
    event_list(&state);
    printf("%d\n", state);
    event_list = fp_arr[0];
    event_list(&state);
    printf("%d\n", state);
    event_list = fp_arr[1];
    event_list(&state);
    printf("%d\n", state);
    return 0;
}

void btnPress(int *current_state) { 
    switch(*current_state) {
        case CLOSED:
            *current_state = OPENING;
            break;
        case OPEN:
            *current_state = CLOSING;
            break;
        case CLOSING:
        case OPENING:
            *current_state = STOPPED;
            break;
        case STOPPED:
            *current_state = UNKNOWN;
            break;
        default:
            *current_state = UNKNOWN;
            break;
    }
}

void virtualBtnPress(int *current_state) {
    switch(*current_state) {
        case CLOSED:
            *current_state = OPENING;
            break;
        case OPEN:
            *current_state = CLOSING;
            break;
        case CLOSING:
        case OPENING:
            // Don't allow virtual button press while door in motion
            break;
        case STOPPED:
            *current_state = UNKNOWN;
            break;
        default:
            *current_state = UNKNOWN;
            break;
    }
}

int confirmSensors(int expectedOpenSW, int expectedClosedSW, Sensors *myDoor) {
    if( (expectedOpenSW == myDoor->openSW) && (expectedClosedSW == myDoor->closedSW) ) {
        return 1;
    } else {
        return 0;
    }
}

    // Now we have defined a type called action_handler that takes two pointers and returns a int
//    typedef  int (*action_handler_t)(void *ctx, void *data);

    // The function pointer to the action looks like a simple type and typedef primarily serves this purpose.
//    typedef struct {
//        state_t curr_state;             /* Enum for the Current state     */
//        event_t event;                  /* Enum for the event             */
//        state_t next_state;             /* Enum for the next state        */
//        action_handler_t event_handler; /* Function-pointer to the action */
//     } state_element;

    // All my event handlers now should adhere to the type defined by action_handler
//    int handle_event_a(void *fsm_ctx, void *in_msg );
//    int handle_event_b(void *fsm_ctx, void *in_msg );

    