#pragma once
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


#include <iostream>
#include <cmath>
#include "botConstants.h"


class polarCoordinates {
public:
    float radius;
    float angle;

    polarCoordinates(float radius_, float angle_): radius(radius_), angle(angle_) {}
};

float distanceBetween(float, float, float, float);

float angleBetween(float, float, float, float);

float angleRangeLimit(float);

class velocity {
public:
    float speed;
    float angle;

    velocity(float speed_, float angle_): speed(speed_), angle(angle_) {}
};

velocity joystickToVelocity(float, float);

velocity velocityAddition(velocity, velocity);

velocity botToWheelVelocity(polarCoordinates, polarCoordinates, float, velocity, float *);

void normalizingSpeeds(float, float, float, float, float, float, float *);

int speedDirectionOptimization(float, float);

float angleDirectionOptimization(float, int);


