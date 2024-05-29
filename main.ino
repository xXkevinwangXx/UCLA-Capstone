//5.8 seconds for 360 turn

//#include <stddef.h>
#include "states.h"
//#include "Sequence.h"
#include <Arduino.h>
//IR Sesnor pin definitions
#include <Servo.h> // Include the Servo library
Servo claw;  // Create a servo object to control a servo
//servo pos 0 = FULLY OPEN
//servo pos 150 = NEARLY CLOSED

int closed = 130;
int open = 0;
int ninety = 750;
int halfway = 700;

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
const int echoPin = 7; // Echo pin connected to digital pin 7

states us(trigPin, echoPin);
states rhs(RIN1, RIN2, RIN3, RIN4, RENA, RENB);
states lhs(LIN1, LIN2, LIN3, LIN4, LENA, LENB);
states linAct(LAIN1, LAIN2, LAENB);
//int operation = 0;

int integrator = 0;
int lasterror = 0;
int puckArray[] = { 6, 1, 4, 6 };  //temporary values  drop off location = 6, second pickup = 1, first pickup = 6
int pucks = sizeof(puckArray) - 1;
//index starts at 0 then 1 then 2
int i = 0;
int max = sizeof(puckArray) - 1;  //while i > 1 focus only on pickup pucks
int FourWaysCrossed = 0;
int colorSum = 0;
int distFromWall = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(LIMIT, INPUT_PULLUP);
  digitalWrite(LED, LOW);
  claw.attach(13);  // Attaches the servo on pin 13 to the servo object
  claw.write(open);   // Initially position the servo at 0 degrees
  delay(3000);        // Wait a moment to let the servo reach the position
}

void loop() 
{
  digitalWrite(LED, LOW);
  if(i <= (pucks-1))
  {
    double nextBranch = ceil((double)puckArray[i] / 2);
    bool fourway = fourWay(colorSum);    
    if (nextBranch > FourWaysCrossed) 
    {
      if (fourway)
      {
        digitalWrite(LED, HIGH);
        while(fourWay(colorSum))
        {
          PIDfollow(integrator, lasterror, lhs, rhs);
        }
        PIDfollow(integrator,lasterror, lhs, rhs);
        delay(halfway - 10);
        lhs.stopMoving();
        rhs.stopMoving();
        FourWaysCrossed++;
        digitalWrite(LED, LOW);
      }
      else PIDfollow(integrator, lasterror, lhs, rhs);
    }
    else if (nextBranch < FourWaysCrossed)
    {
      if (fourway)
      {
        digitalWrite(LED, HIGH);
        while(fourWay(colorSum))
        {
          PIDfollowBW(integrator, lasterror, lhs, rhs);
        }
        digitalWrite(LED, LOW);
        while(!fourWay(colorSum))
        {
          PIDfollowBW(integrator, lasterror, lhs, rhs);
        }
        digitalWrite(LED, HIGH);
        PIDfollow(integrator, lasterror, lhs, rhs);
        delay(halfway);
        lhs.stopMoving();
        rhs.stopMoving();
        digitalWrite(LED, LOW);
        FourWaysCrossed--;
      }
      else PIDfollowBW(integrator, lasterror, lhs, rhs);
    }
    else
    {
      if (puckArray[i] % 2 != 0) 
      {                    //odd number puck turn right to return to main
        // lhs.moveBackward(255);
        // rhs.moveForward(255);
        // delay(ninety);
        leftTurn(lhs, rhs, ninety);
      } 
      else 
      {  //even puck turn right
        // lhs.moveForward(255);
        // rhs.moveBackward(255);
        // delay(ninety);
        rightTurn(lhs, rhs, ninety);
      }
      // linAct.incLength(200);       //extend linAct to max
      delay(350);
      while(!pushed())
      {
        PIDfollow(integrator, lasterror, lhs, rhs);
      }
      lhs.stopMoving();
      rhs.stopMoving();
      delay(2000);
      // claw.write(open);
      // delay(3000);
      // while (pushed() == false) 
      // {  //while the switch isn't push dec length
      //   linAct.decLength(200);
      // }
      // claw.write(closed);
      // delay(3000);
      // linAct.incLength(200);
      // delay(150);
      while(!fourWay(colorSum))
      {
        PIDfollowBW(integrator, lasterror, lhs, rhs);
      }
      PIDfollow(integrator, lasterror, lhs, rhs);
      delay(halfway);

      if (puckArray[i] % 2 != 0) 
      {                    //reverse the turn based off of evenness of current puck
        // lhs.moveForward(255);
        // rhs.moveBackward(255);
        // delay(ninety);
        rightTurn(lhs, rhs, ninety + 300);
      } 
      else 
      {  //even puck turn left
        // lhs.moveBackward(200);
        // rhs.moveForward(255);
        // delay(ninety);
        leftTurn(lhs, rhs, ninety);
      }      
      i++;

    }

  }
  else
  {
    //  past puck collecting phase
    PIDfollow(integrator, lasterror, lhs, rhs);
  }
}
