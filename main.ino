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

int closed = 160;
bool rampClimbed = false;
int open = 0;
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
int LIMIT1 = 24;

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
int puckArray[] = { 3, 2, 5, 4 };                            //temporary values  drop off location = 6, second pickup = 1, first pickup = 6
int pucks = (sizeof(puckArray) / sizeof(puckArray[0])) - 1;  //pucks is now = 3
//index starts at 0 then 1 then 2
int i = 10;
int FourWaysCrossed = 0;
int colorSum = 0;
int distFromWall = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(LIMIT, INPUT_PULLUP);
  pinMode(24, INPUT_PULLUP);
  digitalWrite(LED, LOW);
  linAct.incLength(255);
  delay(2000);
  claw.attach(13);   // Attaches the servo on pin 13 to the servo object
  claw.write(open);  // Initially position the servo at 0 degrees
  delay(3000);       // Wait a moment to let the servo reach the position
}

void loop() {
  digitalWrite(LED, LOW);
  if (i == 10)  //ONLY FOR start to end first corner turn
  {
    if (us.distance() < 13)
    {
      lhs.stopMoving();
      rhs.stopMoving();
    }
    else if (leftAble()) 
    {
      digitalWrite(LED, HIGH);
      leftTurn(lhs, rhs, ninety);
      digitalWrite(LED, LOW);
      i = 0;
      lefts++;
    }
    else PIDfollow(integrator, lasterror, lhs, rhs);
  } 
  else if (i <= (pucks - 1))  //pucks - 1 = 2, i initialized to 0
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
      while (pushedLower() == false) {  //CHANGED FROM US TO LIMIT SWITCH
        digitalWrite(LED, HIGH);
        PIDfollow(integrator, lasterror, lhs, rhs);
      }
      digitalWrite(LED, LOW);
      lhs.stopMoving();
      rhs.stopMoving();
      linAct.decLength(255);
      delay(halfway + 300);
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

  } 
  else  //i = 3 center on branch 2
  {
    if ((us.distance() < 13)) 
    {
      lhs.stopMoving();
      rhs.stopMoving();
    }
    else if (leftAble())
    {
      digitalWrite(LED, HIGH);
      PIDfollow(integrator, lasterror, lhs, rhs);
      delay(halfway);
      leftTurn(lhs, rhs, ninety);
      digitalWrite(LED, LOW);
      lefts++;
    }
    // else if ((us.distance() < 25))
    // {
    //   while(us.distance() < 25)
    //   {
    //     PIDfollow(integrator, lasterror, lhs, rhs);
    //   }
    //   rampClimbed = true;
    // }
    else if ((lefts == 2) && (puckTurn()))
    {
      digitalWrite(LED, HIGH);      
      while(puckTurn())
      {
        lhs.moveForward(200);
        rhs.moveForward(200);
      }
      PIDfollow(integrator, lasterror, lhs, rhs);
      delay(halfway);
      digitalWrite(LED, LOW);
      rights++;
    }
    else if ((lefts == 2) && (rights == puckArray[i]))
    {
      rightTurn(lhs, rhs, ninety);
      while (pushedLower() == false) {
        PIDfollow(integrator, lasterror, lhs, rhs);
      }
      lhs.stopMoving();
      rhs.stopMoving();
      while(!pushed())
      {
        linAct.decLength(255);  //might not be necessary
      }
      claw.write(open);
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
      lhs.moveForward(255);
      rhs.moveForward(255);
      delay(halfway);
      rightTurn(lhs, rhs, ninety);
      while (us.distance() > 25) {
        PIDfollow(integrator, lasterror, lhs, rhs);
      }
      lhs.stopMoving();
      rhs.stopMoving();
      exit(0);
    }
    else PIDfollow(integrator, lasterror, lhs, rhs);


  }
}
