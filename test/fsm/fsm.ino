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
float avArr[8];

byte noLine = 0;
float line_threshold = 10.0;
byte finishLine = 0;


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

float obtain_position_fromIR()
{
    noLine = 1;
    finishLine = 1;
    float tmpPos = 0.0;
    float finPos = 0.0;
    float valSum = 0.0;
    float tmpVal = 0;
    for (int i=0; i<8; i++) {
      tmpVal = 1024.0 - analogRead(pinArr[i]);
      tmpPos += tmpVal * i;
      if (tmpVal > line_threshold) {
        noLine = 0;
      }
      if (tmpVal < 40) {
        finishLine = 0;
      }
      Serial.print("Sensor ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(tmpVal);
      Serial.print("\t");
      valSum += tmpVal;
    }
    if (!noLine) {
      tmpPos = tmpPos/valSum;
      tmpPos = tmpPos/8.0*1000.0;
      tmpPos = map(tmpPos, 110.0, 750.0, 0.0, 1000.0);
      tmpPos = map(tmpPos, 300.0, 700.0, 0.0, 1000.0);

//      for (int j=0; j<7; j++) {
//        avArr[j] = avArr[j+1];
//      }
//      avArr[7] = tmpPos;
//    
//      for (int k=0; k<8; k++) {
//        finPos += avArr[k];
//      } 
//
//      finPos = finPos/8.0;
      finPos = tmpPos;
      Serial.print(finPos);
      Serial.print("\n");

      return finPos;  //TODO    
    } else {
      Serial.println();
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
 
 
// Implement the line following routine here, exit this function when either:
// The ultrasonic sensor detects an obstacle, returning Grab_state   OR
// The finish line is detected, returning Finished_state
State_type followFunc() {
 
    int ult_counter = 0;
    int no_line_counter = 0;
    
    Serial.println("Entering Line Following State!");
 
    while (true) {
 
        // Detect if an obstacle is in front of the robot, return Grab_state if so
        float ult_res = ultRead(ULT_SEN);
        Serial.println(ult_res);
        if (ult_counter >= 5) {
          return Grab_state;
        }
        else if (ult_res <= 3 || ult_res >= 1100) {
          ult_counter++;
        }
        // Detect if the robot has reached the finish line, return Finished_state if so
//        else if (no_line_counter >= 5) {
//          return Finished_state;
//        }
//        else if (noLine) {
//          no_line_counter++;
//        }

        else if (finishLine) {
          return Finished_state;
        }
        // Perform PID line following
        else {
          ult_counter = 0;
          no_line_counter = 0;
          //line following  if (dt > 1.0/control_freq)
          float dt = (micros() - last_time)/ 1.0e6;
          if (dt > 1.0/control_freq)
          {
            float whereAmI = obtain_position_fromIR();
            float whereDoIWantToGo = 500;                      //TODO
 
            PID_controller.update(whereAmI, whereDoIWantToGo);                     //TODO 
 
            float delta_control = PID_controller.get_control();
    
            float constant_speed = 72;                        //TODO: Default Cruising speed
 
            //Differential Drive
            motorDrive(constant_speed - delta_control, M1IN1, M1IN2); //TODO
            motorDrive(constant_speed + delta_control, M2IN1, M2IN2); //TODO
    
          }
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
