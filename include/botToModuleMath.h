#pragma once


#include <iostream>
#include <cmath>
#include "botConstants.h"


class PolarCoordinates {
public:
    float getAngle() {
        if (angle < 0) {
            angle = fmod(angle, 2 * F_PI);
            angle += 2 * F_PI;
        }
        if (angle > 2 * F_PI) {
            angle = fmod(angle, 2 * F_PI);
        }
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


