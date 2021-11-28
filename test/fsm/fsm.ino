#include <Servo.h>

#define M1IN1       12
#define M1IN2       11
#define M2IN1       10
#define M2IN2       2

#define ULT_SEN     9
#define    VELOCITY_TEMP(temp)       ( ( 331.5 + 0.6 * (float)( temp ) ) * 100 / 1000000.0 ) // The ultrasonic velocity (cm/us) compensated by temperature

const uint8_t CHANNELS = 4;
const int pwmPins[CHANNELS] = {2, 3 , 11, 12};

int pinArr[8] = {A0,A1,A2,A3,A4,A5,A6,A7};

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
float line_threshold = 10;
float int_upper = 128;

typedef enum {
    Follow_state = 0,       // The initial state, robot should be following line
    Grab_state = 1,         // The state when some obstacle is detected, grab obstacle in this state
    Finished_state = 2      // The finish line is detected, stop the robot in this state
} State_type;


/********************************* MOTORS *******************************/

class PID
{
  float Kp, Ki, Kd;
  float error, last_error;
  float integral;
  float out;
  float int_upper;
 
  public:
  PID(float Kp, float Ki, float Kd, float int_upper) : Kp(Kp), Ki(Ki), Kd(Kd), int_upper(int_upper){}
  
  void update(float state, float desired_state)
  {
    last_error = error;
    error = desired_state - state;
    integral += error;
    
    float i_term = Ki * integral;
    if (fabs(i_term) > int_upper) i_term = i_term > 0 ? int_upper : -int_upper;
    
    out = Kp * error + i_term + Kd * (error - last_error);
  }
  float get_control()
  {
    return out;
  }
};


void motorDrive(int power, int in1, int in2) {
  if (power >= 0) {
    analogWrite(in2, 0);
    analogWrite(in1, power);
    } else {
    analogWrite(in1, 0);
    analogWrite(in2, abs(power));
    }
}


/********************************* ULTRASONIC SENSOR *******************************/

int16_t trigechoPin = 9;
float distance;
uint32_t pulseWidthUs;
int16_t  dist, temp;

float ultRead(int pin) {
  pinMode(trigechoPin,OUTPUT);
  digitalWrite(trigechoPin,LOW);

  digitalWrite(trigechoPin,HIGH);//Set the trig pin High
  delayMicroseconds(10);     //Delay of 10 microseconds
  digitalWrite(trigechoPin,LOW); //Set the trig pin Low

  pinMode(trigechoPin,INPUT);//Set the pin to input mode
  pulseWidthUs = pulseIn(trigechoPin,HIGH);//Detect the high level time on the echo pin, the output high level time represents the ultrasonic flight time (unit: us)

  distance = pulseWidthUs * VELOCITY_TEMP(20) / 2.0;//The distance can be calculated according to the flight time of ultrasonic wave,/
                                                    //and the ultrasonic sound speed can be compensated according to the actual ambient temperature
  return distance;

}

//
//// Record the time when the rising and falling edge occurs
//void ultRiseRoutine() {
//    if (trigState == 0) {  // rising edge
//        startTime = micros();
//        trigState = 1;
//    } else {
//        endTime = micros();
//        trigState = 2;
//    }
//}


/********************************* IR SENSOR *******************************/

void readIR(bool &line_detected, bool &line_started, bool &line_ended, int i) {
    float IR_reading = analogRead(i);
    if (IR_reading > line_threshold) {
        line_detected = true;
        if (!line_ended) {
            line_started = true;
            error = error + i - 4;
        } else {
            if (line_started) {
                line_ended = true;
            }
        }
    }
}


/********************************* SERVO *******************************/

Servo myServo;

void setServo (int ccbCh, float angle) {
    angle = min(180.0, angle);
    angle = max(0.0, angle);
    int pulseWidth = 1000 + angle / 180.0 * 1000;
    switch (ccbCh) {
        case 0:
            REG_TCC0_CCB0 = pulseWidth;
            break;
        case 1:
            REG_TCC0_CCB1 = pulseWidth;
            break;
        case 2:
            REG_TCC0_CCB2 = pulseWidth;
            break;
        case 3:
            REG_TCC0_CCB3 = pulseWidth;
            break;
    }
}


void setupPWM() {
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(3) |          // Divide the 48MHz clock source by divisor 3: 48MHz/3=16MHz
                    GCLK_GENDIV_ID(4);            // Select Generic Clock (GCLK) 4
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization
 
  REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC |           // Set the duty cycle to 50/50 HIGH/LOW
                     GCLK_GENCTRL_GENEN |         // Enable GCLK4
                     GCLK_GENCTRL_SRC_DFLL48M |   // Set the 48MHz clock source
                     GCLK_GENCTRL_ID(4);          // Select GCLK4
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization  
 
  for (uint8_t i = 0; i < CHANNELS; i++)
  {
    // Enable the port multiplexer for the 4 PWM channels: timer TCC0 outputs
    PORT->Group[g_APinDescription[pwmPins[i]].ulPort].PINCFG[g_APinDescription[pwmPins[i]].ulPin].bit.PMUXEN = 1;
    // Connect the TCC0 timer to the port outputs - port pins are paired odd PMUXO and even PMUXE
    // F & E specify the timers: TCC0, TCC1 and TCC2
    PORT->Group[g_APinDescription[pwmPins[i]].ulPort].PMUX[g_APinDescription[pwmPins[i]].ulPin >> 1].reg = PORT_PMUX_PMUXO_F | PORT_PMUX_PMUXE_F;
  }
 
  // Feed GCLK4 to TCC0 and TCC1
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |         // Enable GCLK4 to TCC0 and TCC1
                     GCLK_CLKCTRL_GEN_GCLK4 |     // Select GCLK4
                     GCLK_CLKCTRL_ID_TCC0_TCC1;   // Feed 16 MHz GCLK4 to TCC0 and TCC1
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization
 
  // Single slope PWM operation: timers countinuously count up to PER register value
  REG_TCC0_WAVE |= TCC_WAVE_WAVEGEN_NPWM;         // Setup normal PWM on TCC0                   
  while (TCC0->SYNCBUSY.bit.WAVE);                // Wait for synchronization
 
  // Divide the 16MHz signal by 16 giving 1MHz (1us) TCC0 timer tick and enable the outputs
  REG_TCC0_CTRLA |= TCC_CTRLA_PRESCALER_DIV16 |   // Divide GCLK4 by 16
                    TCC_CTRLA_ENABLE;             // Enable the TCC0 output
  while (TCC0->SYNCBUSY.bit.ENABLE);              // Wait for synchronization
}


/********************************* MAIN *******************************/

const float control_freq = 20;
float last_time;
PID PID_controller(0.48,0.0,0.0,128);
void setup() {
    // Place your init code here
//    setupPWM();
//    setServo(1, 0);
    myServo.attach(3);
    myServo.write(0);
    
    Serial.begin(9600);
 
}
 
 
bool linePosition(char direction) {
    bool line_detected = false;   
    bool line_started = false;
    bool line_ended = false;

    direction = 'l'; // * test

    if (direction == 'l') {
        for (int i = 0; i < 8; i++) {
            readIR(line_detected, line_started, line_ended, i);
        }
    } else if (direction == 'r') {
        for (int i = 7; i >= 0; i--) {
            readIR(line_detected, line_started, line_ended, i);
        }
    }

    //TODO: check if the node is passed and update the direction_index

    P_error = Kp * error;
    
    integral += error;
    I_error = Ki * integral;
    if (fabs(I_error) > int_upper) I_error = I_error > 0 ? int_upper : -int_upper;

    D_error = Kd * (error - last_error);
    last_error = error;

    float out = P_error + I_error + D_error;

    motorDrive(cruising_speed + out, M1IN1, M1IN2);
    motorDrive(cruising_speed - out, M2IN1, M2IN2);
    
    return line_detected;
}
// Implement the line following routine here, exit this function when either:
// The ultrasonic sensor detects an obstacle, returning Grab_state   OR
// The finish line is detected, returning Finished_state
State_type followFunc() {    
    Serial.println("Entering Line Following State!"); 
    float cur_time = micros();
    float prev_time = cur_time;
    while (true) {
        // if (ultRead()) {
        //     return STATE_BRAKING;
        // }
      cur_time = micros();
      if ((cur_time - prev_time)/1.0e6 > 1.0/control_freq) {
          // if (nodeDistance() <= NODE_BOUNDS) {
          //     releaseEdge(&connectEdge(path[0],path[1]));
          //     publishWeightChanges(weightsJson);

          //     dequeue(path, path_length);
          //     dequeue(direction_queue, direction_length);
              
          // }
          if (!linePosition(direction_queue[0])) {
              return STATE_ARRIVED;
          }
          prev_time = cur_time;
      }
    }
}
 
// Implement the grabbing routine here, exit this function when the robot
// is finished grabbing the obstacle in front of it, and return Follow_state
State_type grabFunc() {
 
    Serial.println("Entering Grabbing State!");
 
    motorDrive(0, M1IN1, M1IN2);
    motorDrive(0, M2IN1, M2IN2);
    // Perform grabbing action here

    //setServo(1,0);
    myServo.write(180);
    delay(1000);

    motorDrive(120, M1IN1, M1IN2);
    motorDrive(-120, M2IN1, M2IN2);
    delay(1000);

    motorDrive(0, M1IN1, M1IN2);
    motorDrive(0, M2IN1, M2IN2);
    
    //setServo(1,180);
    myServo.write(0);
    delay(1000);

    motorDrive(-120, M1IN1, M1IN2);
    motorDrive(120, M2IN1, M2IN2);
    delay(1000);

    motorDrive(0, M1IN1, M1IN2);
    motorDrive(0, M2IN1, M2IN2);
    delay(1000);
    
    return Follow_state;
    
}
 
// Implement a routine that halts the robot, this state does not lead to other states
State_type stopFunc() {
 
    Serial.println("Entering Stop State!");
 
    // Stop the motors here
    motorDrive(0, M1IN1, M1IN2);
    motorDrive(0, M2IN1, M2IN2);
 
    while(true) {}  // Code execution will be stuck here after stopping the robot
    
}
 
void loop() {
 
    State_type robotState = Follow_state;
 
    while (true) {
        switch (robotState) {
            case Follow_state:
                robotState = followFunc();
                break;
 
            case Grab_state:
                robotState = grabFunc();
                break;
 
            case Finished_state:
                robotState = stopFunc();
                break;
            
            default:
                Serial.println("ERROR! UNKNOWN STATE!");
                break;
            }
    }
 
}
