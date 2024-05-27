//5.8 seconds for 360 turn

//#include <stddef.h>
#include "states.h"
//#include "Sequence.h"
#include <Arduino.h>
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
int RENA = 10;  //replace # with proper pin number
int RENB = 9;   //replace # with proper pin number

// LHS Bridge
int LIN1 = 33;
int LIN2 = 31;
int LIN3 = 29;
int LIN4 = 27;
int LENA = 12;  //replace # with proper pin number
int LENB = 11;  //replace # with proper pin number

//Linear Actuator Bridge replace # with proper pin number
int LAIN1 = 45;
int LAIN2 = 47;
int LAENB = 6;

states rhs(RIN1, RIN2, RIN3, RIN4, RENA, RENB);
states lhs(LIN1, LIN2, LIN3, LIN4, LENA, LENB);
states linAct(LAIN1, LAIN2, LAENB);
//int operation = 0;

int integrator = 0;
int lasterror = 0;
int puckArray[] = { 6, 1, 6 };  //temporary values  drop off location = 6, second pickup = 1, first pickup = 6
//index starts at 0 then 1 then 2
int i = sizeof(puckArray) / sizeof(puckArray[0]);  //while i > 1 focus only on pickup pucks
int nextBranch = ceil(puckArray[i - 2] / 2);
int FourWaysCrossed = 0;
int colorSum = 0;
int distFromWall = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print(3);
  delay(10000);
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
  /*Pseudocode logic:
  First part = from starting robot to robot picking up all pucks and drive past all branches
  Values to track: distance from wall, current intersection & intersections passed, pucks to pick up (array size), 
  if array size = 3 and not at fourway then line follow
  check if puckup 1 or 2 pucks
  check if correct fourway intersection
  check if puck to be picked is odd or even
  */
  bool fourway = fourWay(colorSum);  //is this proper usage of passing through parameter?
  Serial.println(colorSum);
  int distFromWall = distance();         //continuously retrieve distance from wall
  Serial.println(String(distFromWall));  //debugging fine tuning
  Serial.print(" cm");
  if (i == 3) {              //if still have pucks to pickup. i will be equal to 3 until it needs to determine whether or not to turn left or right onto main branch for next puck
    if (fourway == false) {  //if still have pucks to pickup and not at fourway
      PIDfollow(integrator, lasterror, lhs, rhs);
      if (distFromWall < 10) {       //appropriate distance fromwall
        linAct.incLength(150);       //extend linAct to max
        while (pushed() == false) {  //while the switch isn't push dec length
          linAct.decLength(150);
        }
        //code to close claw Ex. claw.close(150);
        i--;
      }
    } else if (fourway == true) {
      FourWaysCrossed++;
      if (FourWaysCrossed == ceil(puckArray[i - 1] / 2)) {  //if current branch = destination branch
        if (puckArray[i - 1] % 2 != 0) {                    //odd number puck turn left
          lhs.moveBackward(150);
          rhs.moveForward(150);
          delay(1500);
        } else {  //even puck turn right
          lhs.moveForward(150);
          rhs.moveBackward(150);
          delay(1500);
        }
        //code to go back to main branch robot should be facing on axis perpendicular to main branch ex -|

      } else {
        lhs.moveForward(150);
        rhs.moveForward(150);
        delay(2000);
      }
    }
  }
  
  if



  PIDfollow(integrator, lasterror, lhs, rhs);  //make it go in reverse tho? until it reaches main branch WIP
  if ( nextBranch > FourWaysCrossed) {       //current issue with making sure this isn't being looped
    if (puckArray[2] % 2 == 0) {
      lhs.moveBackward(150);
      rhs.moveForward(150);
      delay(2000);
      lhs.moveForward(150);
      rhs.moveForward(150);
    } else {
      lhs.moveForward(150);
      rhs.moveBackward(150);
      delay(2000);
      lhs.moveForward(150);
      rhs.moveForward(150);
    }
  }
  if (i = 2) {  //only one puck left
    //SECTIONED CODE IS COPY PASTED FROM I=3; vvvv need to make changes so claw opens earlier?
    if (fourway == false) {  //if still have pucks to pickup and not at fourway
      PIDfollow(integrator, lasterror, lhs, rhs);
      if (distFromWall < 10) {       //appropriate distance fromwall
        linAct.incLength(150);       //extend linAct to max
        while (pushed() == false) {  //while the switch isn't push dec length
          linAct.decLength(150);
        }
        //code to close claw Ex. claw.close(150);
        i--;
      }
    } else if (fourway == true) {
      FourWaysCrossed++;
      if (FourWaysCrossed == ceil(puckArray[i - 1] / 2)) {  //if current branch = destination branch
        if (puckArray[i - 1] % 2 != 0) {                    //odd number puck turn left
          lhs.moveBackward(150);
          rhs.moveForward(150);
          delay(1500);
        } else {  //even puck turn right
          lhs.moveForward(150);
          rhs.moveBackward(150);
          delay(1500);
        }
        //code to go back to main branch robot should be facing on axis perpendicular to main branch ex -|

      } else {
        lhs.moveForward(150);
        rhs.moveForward(150);
        delay(2000);
      }
    }
  }
  //SECTIONED CODE IS COPY PASTED FROM I=3; ^^^^
}
else if (nextBranch < FourWaysCrossed) {
  if (puckArray[2] % 2 == 0) {
    rhs.moveBackward(150);
    lhs.moveForward(150);
    delay(2000);
    lhs.moveForward(150);
    rhs.moveForward(150);
  } else {
    rhs.moveForward(150);
    lhs.moveBackward(150);
    delay(2000);
    lhs.moveForward(150);
    rhs.moveForward(150);
  }
}
else if (nextBranch = FourWaysCrossed) {  //spin 180
}
}
else if (i == 1) {  //only a right turn when needing to drop off
  return;
  /*if (distance < 50 && lasterror < -1500) {  //need to turn sharp left
      Serial.println("Sharp left turn");
      //rhs.moveForward(150);
    } else if (distance < 50 && lasterror > 1500) {  //need to turn sharp right
      Serial.println("Sharp right turn");
      //lhs.moveForward(150);
    } else {
      PIDfollow(integrator, lasterror, lhs, rhs);
    }*/
}
