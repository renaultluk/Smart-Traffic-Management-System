#include <SPI.h>
#include "DW1000Ranging.h"

#define LINE_THRESHOLD   50
#define NUM_SENSORS       8
#define INT_UPPER       128

#define M1IN1             8
#define M1IN2            10
#define M2IN1            11
#define M2IN2            12

#define ULT_SEN           9
#define ULT_SEN_THRESHOLD 5

#define PIN_RST           7
#define PIN_SS            4
#define PIN_IRQ           2

const float Kp = 0.1;
const float Ki = 0;
const float Kd = 0;

int error = 0;
int last_error = 0;

float P_error;
float I_error;
float D_error;
float integral = 0;

float cruising_speed = 100;

void motorDrive(int power, int in1, int in2) {
  if (power >= 0) {
    analogWrite(in2, 0);
    analogWrite(in1, power);
    } else {
    analogWrite(in1, 0);
    analogWrite(in2, abs(power));
    }
}

void brake() {
  motorDrive(0, M1IN1, M1IN2);
  motorDrive(0, M2IN1, M2IN2);
}

bool ultRead() {
    float ultReading = analogRead(ULT_SEN);
    return (ultReading <= ULT_SEN_THRESHOLD);
}

void readIR(bool &line_detected, bool &line_started, bool &line_ended, int i) {
    float IR_reading = analogRead(i);
    if (IR_reading > LINE_THRESHOLD) {
        line_detected = true;
        if (!line_ended) {
            line_started = true;
            error = error + i - NUM_SENSORS/2;
        } else {
            if (line_started) {
                line_ended = true;
            }
        }
    }
}

bool linePosition(char direction) {
    bool line_detected = false;   
    bool line_started = false;
    bool line_ended = false;

    if (direction == 'l') {
        for (int i = 2; i < NUM_SENSORS+2; i++) {
            readIR(line_detected, line_started, line_ended, i);
        }
    } else if (direction == 'r') {
        for (int i = NUM_SENSORS+1; i >= 2; i--) {
            readIR(line_detected, line_started, line_ended, i);
        }
    }

    //TODO: check if the node is passed and update the direction_index

    P_error = Kp * error;
    
    integral += error;
    I_error = Ki * integral;
    if (fabs(I_error) > INT_UPPER) I_error = I_error > 0 ? INT_UPPER : -INT_UPPER;

    D_error = Kd * (error - last_error);
    last_error = error;

    float out = P_error + I_error + D_error;

    motorDrive(cruising_speed + out, M1IN1, M1IN2);
    motorDrive(cruising_speed - out, M2IN1, M2IN2);
    
    return line_detected;
}

void initUWB() {
    DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);
    DW1000Ranging.attachNewRange(newRange);
    DW1000Ranging.attachNewDevice(newDevice);
    DW1000Ranging.attachInactiveDevice(inactiveDevice);

    DW1000Ranging.startAsTag();
}

void newRange() {

}

void newDevice(DW1000Device *device) {

}

void inactiveDevice(DW1000Device *device) {

}

void trilateration(DW1000Device *device, float coords[]) {

}

float nodeDistance(DW1000Device *device) {
    float coords[2];
    trilateration(device, coords);
    return sqrt(pow(coords[0], 2) + pow(coords[1], 2));
}