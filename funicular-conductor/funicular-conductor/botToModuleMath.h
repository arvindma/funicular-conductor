#pragma once
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


#include <iostream>
#include <cmath>


class polarCoordinates {
public:
	float radius;
	float angle;

public: polarCoordinates(float x, float y) {
	radius = x;
	angle = y;
}
public: static float distanceBetween(float r1, float theta1, float r2, float theta2) {
	return sqrt(r1 * r1 + r2 * r2 + 2 * r1 * r2 * cos(theta2 - theta1));
}
public: float angleBetween(float r1, float theta1, float r2, float theta2) {
	
	float angleOutput = theta1-asin((r2*sin(theta2-theta1))/(polarCoordinates::distanceBetween(r1, theta1, r2, theta2)));
	cout << angleOutput << endl;
	return angleOutput;
}
public: static float angleRangeLimit(float angle) {
	while (angle < 0) {
		angle = angle + 2 * M_PI;
	}
	while (angle > 2*M_PI) {
		angle = angle - 2 * M_PI;
	}
	return angle;
}

};

class velocity {
	public:
	float speed;
	float angle;

	public: velocity(float x, float y) {
	speed = x;
	angle = y;
}

	public: static velocity joystickToVelocity(float normalizedJoystick, float angle) {
		velocity velocityOutput(normalizedJoystick, angle);
		return velocityOutput; //Add conversion to in/sec
	}
	public: static velocity velocityAddition(velocity v1, velocity v2) {
		if (v1.speed != 0 && v2.speed != 0) {
			velocity velocityOutput(0.0, 0.0);
			velocityOutput.speed = polarCoordinates::distanceBetween(v1.speed, v1.angle, v2.speed, v2.angle);
			velocityOutput.angle = v1.angle + atan2f(v2.speed * sin(v2.angle - v1.angle), v1.speed + v2.speed * cos(v2.angle - v1.angle));
			return velocityOutput;
		}

		else if (v1.speed == 0 && v2.speed == 0) return velocity(0, 0);

		else if (v2.speed == 0) return v1;

		else return v2;
	}
public:
	static velocity botToWheelVelocity(polarCoordinates mp, polarCoordinates cR, float rS, velocity tS) {
		polarCoordinates deltacmp(0.0, 0.0); //Difference between center of rotation and module position (which is nothing when cR is (0,0))
		deltacmp.radius = deltacmp.distanceBetween(mp.radius, mp.angle, cR.radius, cR.angle);
		deltacmp.angle = deltacmp.angleBetween(mp.radius, mp.angle, cR.radius, cR.angle);

		float maxTSpeed = 1;
		float maxRSpeed = deltacmp.radius * 1;

		velocity rotationVector(0.0, 0.0);

		rotationVector.speed = deltacmp.radius * rS;
		rotationVector.angle = deltacmp.angle + (M_PI / 2 * rS / abs(rS));

		velocity prenormalizedOutput = velocity::velocityAddition(rotationVector, tS);

		velocity velocityOutput(0, 0);
		velocityOutput.speed = abs(prenormalizedOutput.speed / (maxRSpeed + maxTSpeed));
		velocityOutput.angle = prenormalizedOutput.angle;
		return velocityOutput;
	}
};


