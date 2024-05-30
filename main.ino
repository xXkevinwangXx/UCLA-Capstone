//5.8 seconds for 360 turn

//#include <stddef.h>
#include "states.h"
//#include "Sequence.h"
#include <Arduino.h>
//IR Sesnor pin definitions
#include <Servo.h>  // Include the Servo library
Servo claw;         // Create a servo object to control a servo
//servo pos 0 = FULLY OPEN
//servo pos 150 = NEARLY CLOSED

int closed = 130;
int open = -10;
int ninety = 750;
int halfway = 750;
int lefts = 0;
int rights = 0;

#define IR1 A1
#define IR2 A2
#define IR3 A3
#define IR4 A4
#define IR5 A5

int LED = 5;

// RHS Bridge
int RIN1 = 41;
int RIN2 = 39;
int RIN3 = 37;
int RIN4 = 35;
int RENA = 10;  //replace # with proper pin number
int RENB = 9;   //replace # with proper pin number

// LHS Bridge
int LIN1 = 33;
int LIN2 = 31;
int LIN3 = 29;
int LIN4 = 27;
int LENA = 12;  //replace # with proper pin number
int LENB = 11;  //replace # with proper pin number

int LIMIT = 23;

//Linear Actuator Bridge replace # with proper pin number
int LAIN1 = 45;
int LAIN2 = 47;
int LAENB = 6;

//Ultrasound
const int trigPin = 8;  // Trigger pin connected to digital pin 8
const int echoPin = 7;  // Echo pin connected to digital pin 7

states us(trigPin, echoPin);
states rhs(RIN1, RIN2, RIN3, RIN4, RENA, RENB);
states lhs(LIN1, LIN2, LIN3, LIN4, LENA, LENB);
states linAct(LAIN1, LAIN2, LAENB);
//int operation = 0;

int integrator = 0;
int lasterror = 0;
int puckArray[] = { 6, 1, 4, 6 };                            //temporary values  drop off location = 6, second pickup = 1, first pickup = 6
int pucks = (sizeof(puckArray) / sizeof(puckArray[0])) - 1;  //pucks is now = 3
//index starts at 0 then 1 then 2
int i = 0;
int max = sizeof(puckArray) - 1;
int FourWaysCrossed = 0;
int RightTurnCrossed = 0;
int colorSum = 0;
int distFromWall = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(LIMIT, INPUT_PULLUP);
  digitalWrite(LED, LOW);
  linAct.incLength(255);
  delay(2000);
  claw.attach(13);   // Attaches the servo on pin 13 to the servo object
  claw.write(open);  // Initially position the servo at 0 degrees
  delay(3000);       // Wait a moment to let the servo reach the position
}

void loop() {
  digitalWrite(LED, LOW);
  if (i == 5)  //ONLY FOR start to end first corner turn
  {
    PIDfollow(integrator, lasterror, lhs, rhs);
    if (us.distance() < 25) {
      digitalWrite(LED, HIGH);
      leftTurn(lhs, rhs, ninety);
      digitalWrite(LED, LOW);
      i = 0;
      lefts++;
    }
  } else if (i <= (pucks - 1))  //pucks - 1 = 2, i initialized to 0
  {
    double nextBranch = ceil((double)puckArray[i] / 2);
    bool fourway = fourWay(colorSum);
    if (nextBranch > FourWaysCrossed) {
      if (fourway) {
        digitalWrite(LED, HIGH);
        while (fourWay(colorSum)) {
          PIDfollow(integrator, lasterror, lhs, rhs);
        }
        PIDfollow(integrator, lasterror, lhs, rhs);
        delay(halfway);
        lhs.stopMoving();
        rhs.stopMoving();
        FourWaysCrossed++;
        digitalWrite(LED, LOW);
      } else PIDfollow(integrator, lasterror, lhs, rhs);
    } else if (nextBranch < FourWaysCrossed) {
      if (fourway) {
        digitalWrite(LED, HIGH);
        while (fourWay(colorSum)) {
          PIDfollow(integrator, lasterror, lhs, rhs);
        }
        PIDfollow(integrator, lasterror, lhs, rhs);
        delay(halfway);
        digitalWrite(LED, LOW);
        lhs.stopMoving();
        rhs.stopMoving();
        rightTurn(lhs, rhs, ninety);
        rightTurn(lhs, rhs, ninety);
        FourWaysCrossed--;
      } else {
        rightTurn(lhs, rhs, ninety);
        digitalWrite(LED, HIGH);
        lhs.stopMoving();
        rhs.stopMoving();
        delay(10);
        rightTurn(lhs, rhs, ninety);
        digitalWrite(LED, LOW);
        while (!fourWay(colorSum)) {
          PIDfollow(integrator, lasterror, lhs, rhs);
        }
      }
    } else  //nextbranch = fourwaysCrossed
    {
      if (puckArray[i] % 2 != 0) {
        // lhs.moveBackward(255);
        // rhs.moveForward(255);
        // delay(ninety);
        leftTurn(lhs, rhs, ninety);
      } else {  //even puck turn right
        // lhs.moveForward(255);
        // rhs.moveBackward(255);
        // delay(ninety);
        rightTurn(lhs, rhs, ninety);
      }
      while (us.distance() > 6) {
        digitalWrite(LED, HIGH);
        PIDfollow(integrator, lasterror, lhs, rhs);
      }
      digitalWrite(LED, LOW);
      lhs.stopMoving();
      rhs.stopMoving();
      claw.write(open);
      delay(3000);
      while (!pushed()) {  //while the switch isn't push dec length
        linAct.decLength(255);
      }
      claw.write(closed);
      delay(3000);
      linAct.incLength(255);
      delay(2000);
      lhs.moveBackward(255);
      rhs.moveBackward(255);
      delay(halfway - 100);
      rightTurn(lhs, rhs, ninety);
      while (!fourWay(colorSum)) {
        PIDfollow(integrator, lasterror, lhs, rhs);
      }
      PIDfollow(integrator, lasterror, lhs, rhs);
      delay(halfway);
      if (puckArray[i] % 2 != 0) {  //reverse the turn based off of evenness of current puck
        // lhs.moveForward(255);
        // rhs.moveBackward(255);
        // delay(ninety);
        leftTurn(lhs, rhs, ninety);
      } else {  //even puck turn left
        // lhs.moveBackward(200);
        // rhs.moveForward(255);
        // delay(ninety);
        rightTurn(lhs, rhs, ninety);
      }
      i++;
    }

  } else  //i = 3 center on branch 2
  {

    if (FourWaysCrossed < 3) {  //logic statement for crossing remaining 4ways
      PIDfollow(lasterror, integrator, lhs, rhs);
      if (fourWay(colorSum) == true) {
        while (fourWay(colorSum) == true) {
          PIDfollow(integrator, lasterror, lhs, rhs);
        }
        PIDfollow(integrator, lasterror, lhs, rhs);
        delay(halfway);
        FourWaysCrossed++;
      }
    }
    if (FourWaysCrossed == 3) {
      PIDfollow(lasterror, integrator, lhs, rhs);
      if (lefts == 2 && RightTurnCrossed < puckArray[i] && puckTurn() == true) {
        PIDfollow(integrator, lasterror, lhs, rhs);
        delay(halfway);
        RightTurnCrossed++;
      }
      if (RightTurnCrossed == puckArray[i]) {
        rightTurn(lhs, rhs, ninety);
        while (us.distance() > 6) {
          PIDfollow(integrator, lasterror, lhs, rhs);
        }
        lhs.stopMoving();
        rhs.stopMoving();
        linAct.decLength(255);  //might not be necessary
        delay(2000);
        claw.write(open);
        delay(3000);
        rightTurn(lhs, rhs, ninety);  //do a 180
        while (fourWay(colorSum) == false) {
          PIDfollow(integrator, lasterror, lhs, rhs);
        }
        while (fourWay(colorSum) == true) {
          PIDfollow(integrator, lasterror, lhs, rhs);
        }
        PIDfollow(integrator, lasterror, lhs, rhs);
        delay(halfway);
        rightTurn(lhs, rhs, ninety);

        while (us.distance() > 25) {
          PIDfollow(integrator, lasterror, lhs, rhs);
        }
      }
      if (us.distance() < 25 && lefts < 2) {  //if at turn (may cause issue at slanted turn careful!!)
        leftTurn(lhs, rhs, ninety);
        lefts++;
      }
    }
  }
}