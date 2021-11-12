#define LINE_THRESHOLD   50
#define NUM_SENSORS       7
#define INT_UPPER       128

#define M1IN1             2
#define M1IN2            10
#define M2IN1            11
#define M2IN2            12

#define ULT_SEN           9
#define ULT_SEN_THRESHOLD 5

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

bool linePosition() {
    bool lineDetected = false;   
    bool line_started = false;
    bool line_ended = false;

    for (int i = 2; i < NUM_SENSORS+2; i++) {
        float IR_reading = analogRead(i);
        if (IR_reading > LINE_THRESHOLD) {
            lineDetected = true;
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
    P_error = Kp * error;
    
    integral += error;
    I_error = Ki * integral;
    if (fabs(I_error) > INT_UPPER) I_error = I_error > 0 ? INT_UPPER : -INT_UPPER;

    D_error = Kd * (error - last_error);
    last_error = error;

    float out = P_error + I_error + D_error;

    motorDrive(cruising_speed + out, M1IN1, M1IN2);
    motorDrive(cruising_speed - out, M2IN1, M2IN2);
    
    return lineDetected;
}