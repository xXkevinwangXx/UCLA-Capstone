#include "states.h"
#include "Sequence.h"
#include "Arduino.h"

#define LOW 0

//wheel states. Variable type name states which takes 6 inputes
states::states(int IN1, int IN2, int IN3, int IN4, int ENA, int ENB)
{

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  
  _IN1 = IN1; //forward
  _IN2 = IN2; //back
  _IN3 = IN3; //forward
  _IN4 = IN4; //back
  _ENA = ENA;
  _ENB = ENB;

  Serial.print(7);
}
//linear actuator state
states::states(int IN1, int IN2, int ENB)
{
  pinMode(extendPin, OUTPUT); 
  pinMode(retractPin, OUTPUT);
  pinMode(ENB, OUTPUT);

  extendPin = IN1;
  retractPin = IN2;
  _ENB = ENB;
}

bool states::incLength(int speed)
{
  digitalWrite(extendPin, HIGH);
  digitalWrite(retractPin, LOW);
  analogWrite(_ENB, speed);
  Serial.print("extension executed");   
  return true;
}

bool states::decLength(int speed)
{
  digitalWrite(extendPin, LOW);
  digitalWrite(retractPin, HIGH);
  analogWrite(_ENB, speed);
  Serial.print("retraction executed");   
  return true;
}
bool states::moveForward(int speed)
{
  digitalWrite(_IN1, HIGH); //
  digitalWrite(_IN2, LOW);
  digitalWrite(_IN3, HIGH);
  digitalWrite(_IN4, LOW);
  analogWrite(_ENA, speed);
  analogWrite(_ENB, speed);  
  return true;
}

bool states::moveBackward(int speed)
{
  digitalWrite(_IN1, LOW);
  digitalWrite(_IN2, HIGH);
  digitalWrite(_IN3, LOW);
  digitalWrite(_IN4, HIGH);
  analogWrite(_ENA, speed);
  analogWrite(_ENB, speed);
  return true;  
}

bool states::turnLeft(int speed)
{
  analogWrite(_IN1, HIGH);
  analogWrite(_IN2, LOW);
  analogWrite(_IN3, LOW);
  analogWrite(_IN4, HIGH);
  analogWrite(_ENA, speed);
  analogWrite(_ENB, speed);
  return true;
}

bool states::turnRight(int speed)
{
  analogWrite(_IN1, LOW);
  analogWrite(_IN2, HIGH);
  analogWrite(_IN3, HIGH);
  analogWrite(_IN4, LOW);
  analogWrite(_ENA, speed);
  analogWrite(_ENB, speed);  
  return true;  
}

bool states::stopMoving()
{
  analogWrite(_ENA, 0);
  analogWrite(_ENB, 0);
  return true;  
}


void PIDfollow(int &Integrator, int &lastError, states lhs, states rhs){
  
  float Kp = 0.25;
  float Ki = 0.0032;
  float Kd = 0.6;

  int IR1 = analogRead(A1);
  int IR2 = analogRead(A2);
  int IR3 = analogRead(A3);
  int IR4 = analogRead(A4);
  int IR5 = analogRead(A5);

  int error = IR1 + IR2 - IR4 - IR5;
  int P = error;
  Integrator = Integrator + error;
  int D = error - lastError;
  lastError = error;
  

  float motorspeed = P * Kp + Integrator * Ki + D * Kd;
  int lhsspd = 200 + motorspeed;
  int rhsspd = 200 - motorspeed;
  if (lhsspd > 255)
  {
    lhsspd = 255;
  }
  if (rhsspd > 255)
  {
    rhsspd = 255;
  }
  if (rhsspd < -255)
  {
    rhsspd = -255;
  }
  if (lhsspd < -255)
  {
    lhsspd = -255;
  }
  Serial.print("   Diff: "); Serial.print(motorspeed);   Serial.print("   Error: "); Serial.print(error);
  Serial.print("   Int: "); Serial.print(Integrator);   Serial.print("   Deriv: "); Serial.print(lastError);
  Serial.print(" LHS: "); Serial.print(lhsspd);
  Serial.print(" RHS: "); Serial.print(rhsspd);
  // Print the sensor values for debugging
  Serial.print(" IR1: "); Serial.print(IR1);
  Serial.print(" IR2: "); Serial.print(IR2);
  Serial.print(" IR3: "); Serial.print(IR3);
  Serial.print(" IR4: "); Serial.print(IR4);
  Serial.print(" IR5: "); Serial.println(IR5);

  if(lhsspd < 0)
  {
    lhs.moveBackward(lhsspd);
  }
  else lhs.moveForward(lhsspd);
  if(rhsspd < 0)
  {
    rhs.moveBackward(rhsspd);
  }
  else rhs.moveForward(rhsspd);
  }
  /*Motorspeed adjustment here*/


