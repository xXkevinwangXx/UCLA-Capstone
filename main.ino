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

//Ultrasound pin definitions
const int trigPin = 8;  // Trigger pin connected to digital pin 8
const int echoPin = 7; // Echo pin connected to digital pin 7

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

int switchPin = 23;

//variable name rhs of Type states taking in respective pin parameters
states rhs(RIN1, RIN2, RIN3, RIN4, RENA, RENB);
states lhs(LIN1, LIN2, LIN3, LIN4, LENA, LENB);
//variable name for linear actuator
states linAct(LAIN1, LAIN2, LAENB);
//int operation = 0;

int integrator = 0;
int lasterror = 0;
int puckArray[] = {1, 5, 3}; //temporary values  drop off location = 1, second pickup = 5, first pickup = 3
int i = sizeof(puckArray) / sizeof(puckArray[0]); //while i > 1 focus only on pickup pucks
float FourWaysCrossed = 0;
int colorSum = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print(3);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(switchPin, INPUT_PULLUP);
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
  if(i>1){  //if still have pucks to pickup
    bool fourway = fourWay(colorSum);
    if(fourway == false){ //if still have pucks to pickup and not at fourway
      PIDfollow(integrator, lasterror, lhs, rhs);
    }
    else if (fourway == true){  //if still have pucks to pickup and at fourway
      FourWaysCrossed++;
      Serial.println("Current size is " + String(i));
      if(FourWaysCrossed == ceil(puckArray[i]/2)){  //if current branch = destination branch
        if(puckArray[i]%2 != 0){  //odd number puck turn left
        linAct.incLength(50);
        lhs.moveBackward(255);
        rhs.moveForward(255);
        delay(1500);
        }
        else{                     //even puck turn right
        linAct.incLength(50);
        lhs.moveForward(255);
        rhs.moveBackward(255);
        delay(1500);
        }
      }
      else{
        while(colorSum < 600)
          {lhs.moveForward(150);
          rhs.moveForward(150);}
      }

    }
    //reach first horizontal line
    //check for 4way destination
    //if 4way dest = current dice roll turn left/right
    //else go forward past current 4way and 4way++
  }
  else{
    long duration;
    int distance;
    // Clear the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    
    // Set the trigPin HIGH for 10 microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    // Read the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    
    // Calculate the distance
    distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    
    // Print the distance on the serial monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    
    // Wait for 1 second before taking the next measurement
    delay(500);
    if(distance < 50 && lasterror < -1500){   //need to turn sharp left
      Serial.println("Sharp left turn");
      //rhs.moveForward(150);
    }
    else if(distance < 50 && lasterror > 1500){  //need to turn sharp right
      Serial.println("Sharp right turn");
      //lhs.moveForward(150);
    }
    else{
      PIDfollow(integrator, lasterror, lhs, rhs);
    }
  }
  

  
}