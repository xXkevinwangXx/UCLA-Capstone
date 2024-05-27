#pragma once

#include "Arduino.h"
class states
{
  public:
  //Wheel motors decleration and actions
  states(int IN1, int IN2, int IN3, int IN4, int ENA, int ENB); //motor experiment control
  bool moveForward(int speed);
  bool moveBackward(int speed);
  bool turnLeft(int speed);
  bool turnRight(int speed);
  bool stopMoving();

  //Linear Actuator decleration and actions
  states(int IN1, int IN2, int ENB);
  bool incLength(int speed);
  bool decLength(int speed);

  //pickup pucks

  private:
  int _IN1, _IN2, _IN3, _IN4, _ENA, _ENB, 
  extendPin, retractPin;
};

void PIDfollow(int &Integrator, int &lastError, states lhs, states rhs);
bool fourWay(int sum);
int distance();
bool pushed();

/*
int operate(int operation);

//enableA = pulsewidth modulating pin digital 11 - in1 & in2
//enableB = digital 10 - in3 in4
//7,6,5,4 = 1,2,3,4 respectively

int lineFollow();

int turning();

int firstPatty();

int pattyStack();

int dropOff();

int retryStack();
*/

/*
* hist will be a LIFO stack of movement structs
* movement struct contains four ints, each a wheel motor output, and a classification binary (movement/turn)
* dicerolls is a vector of the three patties and then the dropoff location
* 
* void PIDfollow(){};
* 
* int operate(int operation){
*	switch operation:
*	case lineFollowNum
*		return lineFollow();
*	case turningnum
*		return turning();
*	case droppoffnum
*		return dropoff();
*	case firstPattyNum
*		return firstPatty();
*	case pattyStackNum
*		return pattyStack();
*	case retryStacknum
*		return retryStack();
*	case endNum
*		return 0;
  };
* 
* int lineFollow(int threeways, int fourways, int dicerolls, int i, *int hist)
* {
*	if # of threeways equals dropoff (i.e. right four/right three read black else read white)
*		return dropoffnum;
*	elseif # of fourways equals ceil(dicerolls[i]/2) (i.e. all five IR read black)
*		return turningnum;
*	else
		if any sensors read black
			PIDfollow();
			push motor pin data onto hist stack
			if right four/right three read black and else is white
				while rightmost sensor reads black
					PIDfollow();
				threeways++;
			if all five IR read black
				while all five IR read black
					PIDfollow();
				fourways++;
			return linefollownum;
		else
			pop from hist
			reverse motor pin output from hist
			output reversed numbers to motor
			if right four/right three read black and else is white
				while rightmost sensor reads black
					PIDfollow();
			if all five IR read black
				PID move forward until just past intersection
				while all five IR read black
					PIDfollow();
			return linefollownum;
* }
* 
* int turning(int dicerolls, int rollnum)
* {
*	int startang = fetch positional sensor angle
*	if dicerolls[rollnum] is even
*		while (fetch positional sensor angle - start ang) < 88		
			turn right
*	else
* 		while (fetch positional sensor angle - start ang) > -88
*			turn left
*	while ultrasonic sensor > ___ distance
*		PIDfollow();
*	if rollnum == 0
		return firstPattyNum;
	else
		return pattyStackNum;
* }
* 
* int firstPatty(){
*	while button unpressed
*		lower claw
*	close claw for n seconds
*	if current sensor indicates torque is too low
*		while(!retryPatty())
*		return firstPattyNum;
*	else
*		lift claw
*		pop hist and reverse until all sensors black
		pop hist until turn is reversed
		while all sensors black
			PIDfollow();
*		rollnum++;
*		return linefollownum;
  }

  int pattyStack()
  {
	lower claw for number - rollnum*pattythickness
	open claw
*	while button unpressed
*		lower claw
*	close claw for n seconds
*	if current sensor indicates torque is too low
*		return retryPattyNum;
*	else
*		lift claw
*		pop hist and reverse until all sensors black
		pop hist until turn is reversed
		while all sensors black
			PIDfollow();
*		rollnum++;
*		return linefollownum;
  }

  int dropOff()
  {
	int startAng = fetch positional sensor angle
	while (fetch positional angle - startAng) < 88
		turn right
	while ultrasonic > ___ dist
		PIDfollow();
	while button unpressed
		lower claw
	open claw
	pop hist stack until turn reversed
	return endNum;
  }

  int retryStack()
  {
	open claw
	pop hist stack until turn is read
	while ultrasonic > ___ dist
		PIDfollow();
	if rollnum == 0
		return firstPattyNum;
	else
		return pattyStackNum;
  }

*/



/*
* int operation will be initialized at linefollownum
* 
* main loop will be
* operation = operate(operation);
*/