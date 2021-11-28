#include "client.h"
#include <SPI.h>

#define LINE_THRESHOLD    5
#define LED_THRESHOLD    50
#define NUM_SENSORS       8
#define INT_UPPER       128

#define M1IN1            12
#define M1IN2            11
#define M2IN1            10
#define M2IN2            2

#define ULT_SEN           9
#define ULT_SEN_THRESHOLD 5
#define    VELOCITY_TEMP(temp)       ( ( 331.5 + 0.6 * (float)( temp ) ) * 100 / 1000000.0 )


#define PIN_RST           3
#define PIN_SS            SS
#define PIN_IRQ           2

//const float x1 = 0.0;
//const float y1 = 0.0;
//const float x2 = 0.0;
//const float y2 = 0.0;
//const float x3 = 0.0;
//const float y3 = 0.0;

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

float distance;
uint32_t pulseWidthUs;
int16_t  dist, temp;

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
    pinMode(ULT_SEN,OUTPUT);
    digitalWrite(ULT_SEN,LOW);

    digitalWrite(ULT_SEN,HIGH);
    delayMicroseconds(10);     
    digitalWrite(ULT_SEN,LOW);

    pinMode(ULT_SEN,INPUT);
    pulseWidthUs = pulseIn(ULT_SEN,HIGH);

    distance = pulseWidthUs * VELOCITY_TEMP(20) / 2.0;

    return distance <= ULT_SEN_THRESHOLD;
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
        for (int i = 0; i < NUM_SENSORS; i++) {
            readIR(line_detected, line_started, line_ended, i);
        }
    } else if (direction == 'r') {
        for (int i = NUM_SENSORS-1; i >= 0; i--) {
            readIR(line_detected, line_started, line_ended, i);
        }
    }

    int tmpCount = 0;
    for (int i = 1; i < 8; i++) {
      tmpCount += avArr[i];
      avArr[i-1] = avArr[i];
     }
     avArr[7] = error;
     tmpCount += error;

     error = tmpCount/8;
     error -= 2;

    //TODO: check if the node is passed and update the direction_index

    P_error = Kp * error;
    
    // integral += error;
    // I_error = Ki * integral;
    // if (fabs(I_error) > INT_UPPER) I_error = I_error > 0 ? INT_UPPER : -INT_UPPER;

    // D_error = Kd * (error - last_error);
    // last_error = error;

    float out = P_error;

    motorDrive(cruising_speed - out, M1IN1, M1IN2);
    motorDrive(cruising_speed + out, M2IN1, M2IN2);
    
    return line_detected;
}

void setup_lampposts() {
  
}

void initUWB() {
//    DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);
//    DW1000Ranging.attachNewRange(newRange);
//    DW1000Ranging.attachNewDevice(newDevice);
//    DW1000Ranging.attachInactiveDevice(inactiveDevice);
//
//    DW1000Ranging.startAsTag();
}

void newRange() {

}

void newDevice(DW1000Device *device) {

}

void inactiveDevice(DW1000Device *device) {

}

float getDistance(DW1000Device *anchor, DW1000Device *tag) {
    float distance = 0;
    return distance;
}

void trilateration(DW1000Device *device, float coords[]) {
    float r1 = getDistance(lamppost_arr[0].anchor, device);
    float r2 = getDistance(lamppost_arr[1].anchor, device);
    float r3 = getDistance(lamppost_arr[2].anchor, device);
    
    // float A = -2*x1 + 2*x2;
    // float B = -2*y1 + 2*y2;
    // float C = pow(r1,2) - pow(r2,2) - pow(x1,2) + pow(x2,2) - pow(y1,2) + pow(y2,2);
    // float D = -2*x2 + 2*x3;
    // float E = -2*y2 + 2*y3;
    // float F = pow(r2,2) - pow(r3,2) - pow(x2,2) + pow(x3,2) - pow(y2,2) + pow(y3,2);

    float A = -2*lamppost_arr[0].x + 2*lamppost_arr[1].x;
    float B = -2*lamppost_arr[0].y + 2*lamppost_arr[1].y;
    float C = pow(r1,2) - pow(r2,2) - pow(lamppost_arr[0].x,2) + pow(lamppost_arr[1].x,2) - pow(lamppost_arr[0].y,2) + pow(lamppost_arr[1].y,2);
    float D = -2*lamppost_arr[1].x + 2*lamppost_arr[2].x;
    float E = -2*lamppost_arr[1].y + 2*lamppost_arr[2].y;
    float F = pow(r2,2) - pow(r3,2) - pow(lamppost_arr[1].x,2) + pow(lamppost_arr[2].x,2) - pow(lamppost_arr[1].y,2) + pow(lamppost_arr[2].y,2);

    coords[0] = (C*E - F*B)/(E*A - B*D);
    coords[1] = (C*D - A*F)/(B*D - A*E);
}

float nodeDistance(DW1000Device *device, Node* node) {
    float coords[2];
    trilateration(device, coords);
    return sqrt(pow(coords[0] - node->x, 2) + pow(coords[1] - node->y, 2));

}
