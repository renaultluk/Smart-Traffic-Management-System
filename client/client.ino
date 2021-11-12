#include "../graphStructs.h"
#include "credentials.h"
#include "client.h"

#define CONTROL_FREQ    20

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


State planFunc() {
    planPath(start, target, path);
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
            if (!linePosition) {
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
        String input = Serial.readStringUntil('\n');
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