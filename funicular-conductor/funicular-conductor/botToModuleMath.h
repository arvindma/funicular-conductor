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
	return atan((r1 * sin(theta1) + r2 * sin(theta2)) / (r1 * cos(theta1) + r2 * cos(theta2)));
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
		return velocity(normalizedJoystick, angle); //Add conversion to in/sec
	}
	public: static velocity velocityAddition(velocity v1, velocity v2) {
		if (v1.speed > 0) {
			velocity velocityOutput(0.0, 0.0);
			velocityOutput.speed = polarCoordinates::distanceBetween(v1.speed, v1.angle, v2.speed, v2.angle);
			velocityOutput.angle = v1.angle + atan2f(v2.speed * sin(v2.angle - v1.angle), v1.speed + v2.speed * cos(v2.angle - v1.angle));
			return velocityOutput;
		}
		else return v2;
	}
public:
	static velocity botToWheelVelocity(polarCoordinates mp, polarCoordinates cR, float rS, velocity tS) {
		polarCoordinates deltacmp(0.0, 0.0); //Difference between center of rotation and module position
		deltacmp.radius = deltacmp.distanceBetween(mp.radius, mp.angle, cR.radius, cR.angle);
		deltacmp.angle = deltacmp.angleBetween(mp.radius, mp.angle, cR.radius, cR.angle);

		velocity rotationVector(0.0, 0.0);

		rotationVector.speed = deltacmp.radius * rS;
		rotationVector.angle = deltacmp.angle + M_PI / 2 * rS / abs(rS);

		return velocity::velocityAddition(rotationVector, tS);
	}
};


