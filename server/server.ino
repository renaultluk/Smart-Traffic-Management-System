#include "../graphStructs.h"
#include "credentials.h"

void serialInputHandler() {
    char command = Serial.read();
    switch (command) {
        case 'd':
            int vehicleId = Serial.parseInt();
            Node newTarget;
            newTarget.index = Serial.parseInt();
            break;
    
        case 't':
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