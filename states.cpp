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
  Serial.print("moveForward executed");   
  return true;
}

bool states::moveBackward(int speed)
{
  analogWrite(_IN1, LOW);
  analogWrite(_IN2, HIGH);
  analogWrite(_IN3, LOW);
  analogWrite(_IN4, HIGH);
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

/*
void PIDfollow(int &Integrator, int &lastError){
  Kp = 0.07;
  Ki = 0.0008;
  Kd = 0.6;

  int IR1 = analogRead(A1);
  int IR2 = analogRead(A1);
  int IR3 = analogRead(A3);
  int IR4 = -analogRead(A4);
  int IR5 = -analogRead(A5);

  int error = sum(IR1, IR2, IR4, IR5);
  int P = error;
  Integrator = Integrator + error;
  int D = error - lastError;
  lastError = error;

  float motorspeed = P * Kp + I * Ki + D * Kd;
  }*/
  /*Motorspeed adjustment here*/


