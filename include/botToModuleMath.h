#pragma once


#include <iostream>
#include <cmath>
#include "botConstants.h"


class PolarCoordinates {
public:
    float getAngle() {
        float tempAngle = angle;
        if (tempAngle < 0) {
            tempAngle = fmod(tempAngle, 2 * F_PI);
            tempAngle += 2 * F_PI;
        }
        if (tempAngle > 2 * F_PI) {
            tempAngle = fmod(tempAngle, 2 * F_PI);
        }
        return tempAngle;
    }
    float getAngleUnconstrained() {
        return angle;
    }
    void setAngle(float angle_) {
        angle = angle_;
    }

    PolarCoordinates(): magnitude(0), angle(0) {}
    PolarCoordinates(float magnitude_, float angle_): magnitude(magnitude_), angle(angle_) {}

public:
    float magnitude;

private:
    float angle;
};


class Velocity : public PolarCoordinates {
public:
    Velocity() : PolarCoordinates() {}
    Velocity(float magnitude_, float angle_) : PolarCoordinates(magnitude_, angle_) {}
};

float distanceBetween(float, float, float, float);

float angleBetween(float, float, float, float);

Velocity joystickToVelocity(float, float);

Velocity velocityAddition(Velocity *, Velocity*);

Velocity botToWheelVelocity(PolarCoordinates, PolarCoordinates, float, Velocity, float *);

void normalizingSpeeds(float, float, float, float, float, float, float *);

int speedDirectionOptimization(float, float);

float angleDirectionOptimization(float, int);


