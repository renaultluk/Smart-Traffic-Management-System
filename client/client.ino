#include "../graphStructs.h"
#include "credentials.h"
#include "client.h"

#define CONTROL_FREQ    20

#define VEHICLE_ID      1

typedef enum {
    STATE_INIT,
    STATE_CONNECTING,
    // STATE_CONNECTED,
    // STATE_DISCONNECTED,
    // STATE_ERROR,
    STATE_PLANNING,
    STATE_FOLLOWING,
    STATE_BRAKING,
    STATE_ARRIVED,
} State;

enum { MANUAL, AUTO } destMode;

Node start;
Node target;
Node path[NUM_NODES];
char direction_queue[NUM_NODES];
int direction_index = 0;

void serialInputHandler() {
    char command = Serial.read();
    switch (command) {
        case 'd':
            if (Serial.parseInt() == VEHICLE_ID) {
                target.index = Serial.parseInt();
            }
            break;
    
        case 't':
            if (Serial.parseInt() == VEHICLE_ID) {
                destMode = destMode == AUTO ? MANUAL : AUTO;
            }
            break;
        
        default:
            break;
    }
}

State planFunc() {
    planPath(start, target, path, direction_queue);
    return STATE_FOLLOWING;
}

State followFunc() {
    float cur_time = micros();
    float prev_time = cur_time;
    while (true) {
        if (ultRead()) {
            return STATE_BRAKING;
        }
        cur_time = micros();
        if ((cur_time - prev_time)/1.0e6 > 1.0/CONTROL_FREQ) {
            if (!linePosition(direction_queue[direction_index], direction_index)) {
                return STATE_ARRIVED;
            }
            prev_time = cur_time;
        }
    }
}

State brakeFunc() {
    while (ultRead()) {
        brake();
    }
    return STATE_FOLLOWING;
}

State arrivedFunc() {
    switch (destMode)
    {
    case MANUAL:
        brake();
        while (!Serial.available()) {
            delay(100);
        };
        serialInputHandler();
        return STATE_PLANNING;
        break;
    
    case AUTO:
        break;

    default:
        break;
    }
}

void setup() {

}

void loop() {
    State vehicleState = STATE_PLANNING;

    while (true) {
        if (Serial.available()) {
            serialInputHandler();
        }

        switch (vehicleState)
        {
        case STATE_PLANNING:
            vehicleState = planFunc();
            break;
        
        case STATE_FOLLOWING:
            vehicleState = followFunc();
            break;

        case STATE_BRAKING:
            vehicleState = brakeFunc();
            break;

        case STATE_ARRIVED:
            vehicleState = arrivedFunc();
            break;

        default:
            break;
        }
    }
}