#include <stdio.h> // printf

#define OFF     0
#define ON      1

typedef void (*fp_doState)(int *x);  /* this defines a type fp_event */

void handle_state_DoorClosed(void);
void handle_state_DoorOpen(void);
void handle_state_DoorClosing(void);
void handle_state_DoorOpening(void);
void handle_state_DoorStopped(void);
void handle_state_DoorUnknown(void);

typedef enum {
    DOORCLOSED,
    DOOROPENING,
    DOOROPEN,
    DOORCLOSING,
    DOORSTOPPED,
    DOORUNKNOWN
} state;

typedef struct {
    state nextState;
    
} stateElement;

//int state = CLOSED;
//void handle_event_ButtonPress(int *current_state);
//void handle_event_VirtualButtonPress(int *current_state);

//fp_event fp_arr[] = {btnPress, virtualBtnPress};

int main(void) {
    fp_event event_list;
    
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
