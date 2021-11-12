#include "../graphStructs.h"
#include "credentials.h"

enum { MANUAL, AUTO } destMode;

void serialInputHandler() {
    char command = Serial.read();
    switch (command) {
        case 'd':
            int vehicleId = Serial.parseInt();
            Node newTarget;
            newTarget.index = Serial.parseInt();
            break;
    
        case 't':
            int vehicleId = Serial.parseInt();
            destMode = destMode == AUTO ? MANUAL : AUTO;
            break;
        
        default:
            break;
    }
}

void setup() {

}

void loop() {
    if (Serial.available()) {
        serialInputHandler();
    }
}