#include <MatrixMath.h>




//libraries
#include <SPI.h>
#include "DW1000Ranging.h"


////////////////////////////// LSE VARIABLE /////////////////////////////////////////

float p[3][2]={{3.0, 4.0},{12.0, 15.0},{15.0, 18.0}};

//float x_rcv[4][1];
float Teta_0[2][1] = {{3.0},{2.0}};
float teta_0[1][2];
float deno[3][1];
float H[4][2];
float H_trans[2][3];
float H_t_by_H[2][2];
float H_inv[3][2];
float H_i_by_H[3][3];
float H_Ht_inv[2][2];
float d[3][1];
float z[3][1];
float y[3][1];
float teta_hat[2][1];
int i,j,k,l,m,n,q;
float x1[3];
float y1[3];
float x11[3];
float y11[3];
float x2[3];
float y2[3];
float H_Hi_Ht[2][3];
float a,b,c;


// connection pins
const uint8_t PIN_RST = 9; // reset pin
const uint8_t PIN_IRQ = 2; // irq pin
const uint8_t PIN_SS = SS; // spi select pin

//declare variables
int tagID;
float tagRange;
float tagPower;

float Range1;
float Power1;
float Range2;
//float Power2;
float Range3;
//float Power3;
float x_rcv[3][1];
float ID1;
float ID2;
float ID3;

void setup() {
  
  Serial.begin(9600);
  delay(1000);
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);
  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachNewDevice(newDevice);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);
  //DW1000Ranging.useRangeFilter(true);
  DW1000Ranging.startAsTag("7D:00:22:EA:82:60:3B:9C", DW1000.MODE_LONGDATA_RANGE_ACCURACY);
  // Wire.begin();
}

void loop() {
  DW1000Ranging.loop();


}

void newRange() {

  tagID = (int) (DW1000Ranging.getDistantDevice()->getShortAddress());
  tagRange = (float) DW1000Ranging.getDistantDevice()->getRange();
  tagPower = (float) DW1000Ranging.getDistantDevice()->getRXPower();
 // Serial.print("FROM: ");Serial.print(tagID);Serial.print("Range: ");Serial.println(tagRange);

 if (tagID == 1) {
   a={tagRange}   ;
 }

   if (tagID == 2) {
   b={tagRange}  ;
  }

  else if (tagID == 3) {
    c={tagRange}  ;
  }

//float a = 2.52; 
Serial.println("From:1 ");Serial.print(a);
Serial.println("From:2 ");Serial.print(b);
Serial.println("From:3 ");Serial.print(c);
}

void newDevice(DW1000Device* device) { }

void inactiveDevice(DW1000Device* device) { }

