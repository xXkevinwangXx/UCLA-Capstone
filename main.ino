//#include <stddef.h>
#include "states.h"
//#include "Sequence.h"
//#include <Arduino.h>
//IR Sesnor pin definitions
#define IR1 A1
#define IR2 A2
#define IR3 A3
#define IR4 A4
#define IR5 A5

// RHS Bridge
int RIN1 = 41;
int RIN2 = 39;
int RIN3 = 37;
int RIN4 = 35;
int RENA = 10; //replace # with proper pin number
int RENB = 9; //replace # with proper pin number

// LHS Bridge
int LIN1 = 33;
int LIN2 = 31;
int LIN3 = 29;
int LIN4 = 27;
int LENA = 12; //replace # with proper pin number
int LENB = 11; //replace # with proper pin number

//Linear Actuator Bridge replace # with proper pin number
int LAIN1 = 45;
int LAIN2 = 47;
int LAENB = 6; 

//variable name rhs of Type states taking in respective pin parameters
states rhs(RIN1, RIN2, RIN3, RIN4, RENA, RENB);
states lhs(LIN1, LIN2, LIN3, LIN4, LENA, LENB);
//variable name for linear actuator
states linAct(LAIN1, LAIN2, LAENB);
//int operation = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print(3);
  /*Sequence hist;
  move mov;
  mov.mot1 = 1;
  mov.mot2 = 2;
  mov.mot3 = 3;
  mov.mot4 = 4;
  mov.turn = 0;
  hist.insert(mov);
  move copy;
  Serial.print(mov.mot1);
  if(!hist.pop(copy))
    Serial.print(78);
*/
}

void loop() {
  //pin reading for sensor
  int ir1 = analogRead(IR1);
  int ir2 = analogRead(IR2);
  int ir3 = analogRead(IR3);
  int ir4 = analogRead(IR4);
  int ir5 = analogRead(IR5);

  // Print the sensor values for debugging
  Serial.print("IR1: "); Serial.print(ir1);
  Serial.print(" IR2: "); Serial.print(ir2);
  Serial.print(" IR3: "); Serial.print(ir3);
  Serial.print(" IR4: "); Serial.print(ir4);
  Serial.print(" IR5: "); Serial.println(ir5);

  //analog color sensing is between 0-1023
  int threshold = 500;
    // Line tracing logic
  if (ir1 < threshold || ir2 < threshold) {
    Serial.print("Turning left");
    // Left sensors detect the line, turn left
    //lhs.turnLeft(150);
    //delay(1000);
  } else if (ir4 < threshold || ir5 < threshold) {
    // Right sensors detect the line, turn right
      Serial.print("Turning Right");

    //rhs.turnRight(150);
    //delay(1000);

  } else if (ir3 < threshold) {
    // Middle sensor is on the line, move forward
      Serial.print("Going forward");

    //lhs.moveForward(150);
    //rhs.moveForward(150);
    //delay(1000);

  } else {
    // No sensor detects the line, stop moving
    Serial.print("Stopping");
    //drive.stopMoving();
  }
  //lhs.moveForward(50);
  //rhs.moveForward(50);
  //delay(1000);
}
