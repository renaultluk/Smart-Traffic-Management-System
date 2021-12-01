#include "sensor_obj.h"
#include "client.h"

#define LINE_THRESHOLD    5
#define LED_THRESHOLD    50
#define NUM_SENSORS       8
#define INT_UPPER       128

#define M1IN1            12
#define M1IN2            11
#define M2IN1            10
#define M2IN2            9

#define ULT_SEN           2
#define ULT_SEN_THRESHOLD 5
#define    VELOCITY_TEMP(temp)       ( ( 331.5 + 0.6 * (float)( temp ) ) * 100 / 1000000.0 )

const float Kp = 10;
const float Ki = 0;
const float Kd = 0;

int error = 0;
int last_error = 0;

float P_error;
float I_error;
float D_error;
float integral = 0;

float cruising_speed = 55;
int avArr[8];

float distance;
uint32_t pulseWidthUs;
int16_t  dist, temp;

bool within(float val, float lower, float upper) {
  return (val >= lower && val <= upper);
}

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

void readIR(bool &line_detected, bool &line_started, bool &line_ended, bool &split, int i) {
    float IR_reading = 1024.0 - analogRead(i);
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(IR_reading);
    Serial.print("\t");
    if (within(IR_reading, line_sensors[i].line_lower, line_sensors[i].line_upper)) {
        line_detected = true;
        if (!line_ended) {
            line_started = true;
            error = error + i - NUM_SENSORS/2;
        } else {
          split = true;
        }
    } else {
      if (line_started) {
        line_ended = true;
      }
    }
}

bool linePosition(char direction, bool &split) {
    bool line_detected = false;   
    bool line_started = false;
    bool line_ended = false;

    error = 0;

    if (direction == 'l') {
        for (int i = 0; i < NUM_SENSORS; i++) {
            readIR(line_detected, line_started, line_ended, split, i);
        }
    } else if (direction == 'r') {
        for (int i = NUM_SENSORS-1; i >= 0; i--) {
            readIR(line_detected, line_started, line_ended, split, i);
        }
    }
    Serial.println();

    // int tmpCount = 0;
    // for (int i = 1; i < 8; i++) {
    //   tmpCount += avArr[i];
    //   avArr[i-1] = avArr[i];
    //  }
    //  avArr[7] = error;
    //  tmpCount += error;

    //  error = tmpCount/8.0;
//     error += 2;
     Serial.print("error: ");
     Serial.print(error);
     Serial.print("\t");

    //TODO: check if the node is passed and update the direction_index

    P_error = Kp * error;
    
    // integral += error;
    // I_error = Ki * integral;
    // if (fabs(I_error) > INT_UPPER) I_error = I_error > 0 ? INT_UPPER : -INT_UPPER;

    // D_error = Kd * (error - last_error);
    // last_error = error;

    float out = P_error;
    Serial.print("out: ");
    Serial.println(out);

    motorDrive(cruising_speed - out, M1IN1, M1IN2);
    motorDrive(cruising_speed + out, M2IN1, M2IN2);
    
    return line_detected;
}
