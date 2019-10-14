#include "Arduino.h"
#include "CheapStepper.h"


CheapStepper::CheapStepper () : pins({8,9,10,11}) {
	for (int pin=0; pin<4; pin++){
		pinMode(pins[pin], OUTPUT);
	}
}

CheapStepper::CheapStepper (int in1, int in2, int in3, int in4) : pins({in1,in2,in3,in4}) {
	for (int pin=0; pin<4; pin++){
		pinMode(pins[pin], OUTPUT);
	}
}

void CheapStepper::setRpm (int rpm){

	delay = calcDelay(rpm);
}

void CheapStepper::move (bool clockwise, int numSteps){

	for (int n=0; n<numSteps; n++){
		step(clockwise);
	}
}

void CheapStepper::moveTo (bool clockwise, int toStep){

	// keep to 0-(totalSteps-1) range
	if (toStep >= totalSteps) toStep %= totalSteps;
	else if (toStep < 0) {
		toStep %= totalSteps; // returns negative if toStep not multiple of totalSteps
		if (toStep < 0) toStep += totalSteps; // shift into 0-(totalSteps-1) range
	}
	while (stepN != toStep){
		step(clockwise);
	}
}

void CheapStepper::moveDegrees (bool clockwise, int deg){

	int nSteps = (unsigned long) deg * totalSteps / 360;
	move(clockwise, nSteps);
}

void CheapStepper::moveToDegree (bool clockwise, int deg){

	// keep to 0-359 range
	if (deg >= 360) deg %= 360;
	else if (deg < 0) {
		deg %= 360; // returns negative if deg not multiple of 360
		if (deg < 0) deg += 360; // shift into 0-359 range
	}

	int toStep = deg * totalSteps / 360;
	moveTo (clockwise, toStep);
}


// NON-BLOCKING MOVES

void CheapStepper::newMove (bool clockwise, int numSteps){

	// numSteps sign ignored
	// stepsLeft signed positive if clockwise, neg if ccw

	if (clockwise) stepsLeft = abs(numSteps);
	else stepsLeft = -1 * abs(numSteps);

	lastStepTime = micros();
}

void CheapStepper::newMoveTo (bool clockwise, int toStep){

	// keep toStep in 0-(totalSteps-1) range
	if (toStep >= totalSteps) toStep %= totalSteps;
	else if (toStep < 0) {
		toStep %= totalSteps; // returns negative if toStep not multiple of totalSteps
		if (toStep < 0) toStep += totalSteps; // shift into 0-(totalSteps-1) range
	}

	if (clockwise) stepsLeft = abs(toStep - stepN);
	// clockwise: simple diff, always pos
	else stepsLeft = -1*(totalSteps - abs(toStep - stepN));
	// counter-clockwise: totalSteps - diff, made neg

	lastStepTime = micros();
}

void CheapStepper::newMoveDegrees (bool clockwise, int deg){

	int nSteps = (unsigned long) deg * totalSteps / 360;
	newMove (clockwise, nSteps);
}
