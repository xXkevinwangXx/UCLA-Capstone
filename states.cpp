#include "states.h"
//#include "Sequence.h"
#include "Arduino.h"

#define LOW 0
int wait = 0;
float Kp = 0.13;
float Ki = 0.000;
float Kd = 0.000;


//wheel states. Variable type name states which takes 6 inputes
states::states(int IN1, int IN2, int IN3, int IN4, int ENA, int ENB) {

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  _IN1 = IN1;  //forward
  _IN2 = IN2;  //back
  _IN3 = IN3;  //forward
  _IN4 = IN4;  //back
  _ENA = ENA;
  _ENB = ENB;

  Serial.print(7);
}
//linear actuator state
states::states(int IN1, int IN2, int ENB) {
  pinMode(extendPin, OUTPUT);
  pinMode(retractPin, OUTPUT);
  pinMode(ENB, OUTPUT);

  extendPin = IN1;
  retractPin = IN2;
  _ENB = ENB;
}

bool states::incLength(int speed) {
  digitalWrite(extendPin, HIGH);
  digitalWrite(retractPin, LOW);
  analogWrite(_ENB, speed);
  Serial.print("extension executed");
  return true;
}

bool states::decLength(int speed) {
  digitalWrite(extendPin, LOW);
  digitalWrite(retractPin, HIGH);
  analogWrite(_ENB, speed);
  Serial.print("retraction executed");
  return true;
}

states::states(int trigPin, int echoPin) {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  _trigPin = trigPin;
  _echoPin = echoPin;
}

int states::distance() {
  pinMode(_trigPin, OUTPUT);
  pinMode(_echoPin, INPUT);
  long duration;
  int distance;
  // Clear the trigPin
  digitalWrite(_trigPin, LOW);
  delayMicroseconds(2);

  // Set the trigPin HIGH for 10 microseconds
  digitalWrite(_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(_trigPin, LOW);

  // Read the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(_echoPin, HIGH);

  // Calculate the distance
  distance = duration * 0.034 / 2;  // Speed of sound wave divided by 2 (go and back)

  // Print the distance on the serial monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(100);
  return distance;
}

bool states::moveForward(int speed) {
  digitalWrite(_IN1, HIGH);  //
  digitalWrite(_IN2, LOW);
  digitalWrite(_IN3, HIGH);
  digitalWrite(_IN4, LOW);
  analogWrite(_ENA, speed);
  analogWrite(_ENB, speed);
  return true;
}

bool states::moveBackward(int speed) {
  digitalWrite(_IN1, LOW);
  digitalWrite(_IN2, HIGH);
  digitalWrite(_IN3, LOW);
  digitalWrite(_IN4, HIGH);
  analogWrite(_ENA, speed);
  analogWrite(_ENB, speed);
  return true;
}

bool states::stopMoving() {
  analogWrite(_ENA, 0);
  analogWrite(_ENB, 0);
  return true;
}

bool pushedLower() {
  int sensorVal = digitalRead(24);
  if (sensorVal == HIGH) {
    return false;
  } else {
    return true;
  }
}

bool pushed() {
  int sensorVal = digitalRead(23);
  if (sensorVal == HIGH) {
    return false;
  } else {
    return true;
  }
}

bool fourWay(int &sum)  //returns if on a 4 way intersection
{
  int IR1 = analogRead(A1);
  int IR2 = analogRead(A2);
  int IR3 = analogRead(A3);
  int IR4 = analogRead(A4);
  int IR5 = analogRead(A5);
  sum = IR1 + IR2 + IR3 + IR4 + IR5;
  if (sum < 400) {
    return true;
  } else {
    return false;
  }
}

bool puckTurn() {  //return true when clear right turn only
  int IR1 = analogRead(A1);
  int IR2 = analogRead(A2);
  int IR3 = analogRead(A3);
  int IR4 = analogRead(A4);
  int IR5 = analogRead(A5);
  if (IR1 > 100 && IR2 + IR3 + IR4 + IR5 < 800) {
    return true;
  } else return false;
}

bool leftTurn(states lhs, states rhs, int wait) {
  int IR4 = analogRead(A4);
  //first ninety turn
  rhs.moveForward(255);
  lhs.moveBackward(255);
  delay(wait);
  IR4 = analogRead(A4);
  while (IR4 > 200) {
    rhs.moveForward(255);
    lhs.moveBackward(255);
    IR4 = analogRead(A4);
  }
  return true;
}

bool rightTurn(states lhs, states rhs, int wait) {
  int IR2 = analogRead(A2);
  //first ninety turn
  lhs.moveForward(255);
  rhs.moveBackward(255);
  delay(wait + 100);
  IR2 = analogRead(A2);
  while (IR2 > 200) {
    lhs.moveForward(255);
    rhs.moveBackward(255);
    IR2 = analogRead(A2);
  }
  return true;
}

void PIDfollow(int &Integrator, int &lastError, states lhs, states rhs) {

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
  if (lhsspd > 255) {
    lhsspd = 255;
  }
  if (rhsspd > 255) {
    rhsspd = 255;
  }
  if (rhsspd < -255) {
    rhsspd = -255;
  }
  if (lhsspd < -255) {
    lhsspd = -255;
  }
  Serial.print("   Diff: ");
  Serial.print(motorspeed);
  Serial.print("   Error: ");
  Serial.print(error);
  Serial.print("   Int: ");
  Serial.print(Integrator);
  Serial.print("   Deriv: ");
  Serial.print(lastError);
  Serial.print(" LHS: ");
  Serial.print(lhsspd);
  Serial.print(" RHS: ");
  Serial.print(rhsspd);
  // Print the sensor values for debugging
  Serial.print(" IR1: ");
  Serial.print(IR1);
  Serial.print(" IR2: ");
  Serial.print(IR2);
  Serial.print(" IR3: ");
  Serial.print(IR3);
  Serial.print(" IR4: ");
  Serial.print(IR4);
  Serial.print(" IR5: ");
  Serial.println(IR5);

  if (lhsspd < 0)  //turn left
  {
    lhs.moveBackward(lhsspd);
  } else lhs.moveForward(lhsspd);
  if (rhsspd < 0)  //right right
  {
    rhs.moveBackward(rhsspd);
  } else rhs.moveForward(rhsspd);
}

// void PIDfollowBW(int &Integrator, int &lastError, states lhs, states rhs) {

//   int IR1 = analogRead(A1);
//   int IR2 = analogRead(A2);
//   int IR3 = analogRead(A3);
//   int IR4 = analogRead(A4);
//   int IR5 = analogRead(A5);

//   float KpBW = 0.07;

//   int error = IR1 + IR2 - IR4 - IR5;
//   int P = error;
//   Integrator = Integrator + error;
//   int D = error - lastError;
//   lastError = error;


//   float motorspeed = P * (float)KpBW + Integrator * Ki + D * Kd;
//   int lhsspd = 200 - motorspeed;
//   int rhsspd = 200 + motorspeed;
//   if (lhsspd > 255) {
//     lhsspd = 255;
//   }
//   if (rhsspd > 255) {
//     rhsspd = 255;
//   }
//   if (rhsspd < -255) {
//     rhsspd = -255;
//   }
//   if (lhsspd < -255) {
//     lhsspd = -255;
//   }
//   Serial.print(" LHS: ");
//   Serial.print(lhsspd);
//   Serial.print(" RHS: ");
//   Serial.print(rhsspd);
//   // Print the sensor values for debugging
//   Serial.print(" IR1: ");
//   Serial.print(IR1);
//   Serial.print(" IR2: ");
//   Serial.print(IR2);
//   Serial.print(" IR3: ");
//   Serial.print(IR3);
//   Serial.print(" IR4: ");
//   Serial.print(IR4);
//   Serial.print(" IR5: ");
//   Serial.println(IR5);

//   if (lhsspd < 0)  //turn left
//   {
//     lhs.moveForward(lhsspd);
//   } else lhs.moveBackward(lhsspd);

//   if (rhsspd < 0)  //right right
//   {
//     rhs.moveForward(rhsspd);
//   } else rhs.moveBackward(rhsspd);
// }
/*Motorspeed adjustment here*/