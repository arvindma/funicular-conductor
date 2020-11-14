#pragma once

#include <iostream>
#include <cmath>
#include "botConstants.h"


class PolarCoordinates {
public:
    float getConstrainedAngle() const
    {
        float tempAngle = angle;
        if (tempAngle < 0)
        {
            tempAngle = fmod(tempAngle, 2 * F_PI);
            tempAngle += 2 * F_PI;
        }
        if (tempAngle > 2 * F_PI)
        {
            tempAngle = fmod(tempAngle, 2 * F_PI);
        }
        return tempAngle;
    }

    PolarCoordinates(): magnitude(0), angle(0) {}
    PolarCoordinates(float magnitude_, float angle_): magnitude(magnitude_), angle(angle_) {}

public:
    float magnitude;
    float angle;
};


class Velocity : public PolarCoordinates {
public:
    Velocity() : PolarCoordinates() {}
    Velocity(float magnitude_, float angle_) : PolarCoordinates(magnitude_, angle_) {}
};

class Module
{
public:
    Velocity velocity[3] = { Velocity(0.0, 0.0), Velocity(0.0, 0.0), Velocity(0.0, 0.0) };
    float normalizingFactor = 1;
    int turns [3] = { 0, 0, 0 };
    int directionSwitchAngle[3] = { 0, 0, 0 };
    int directionSwitch[3] = { 1, 1, 1 };
    float angleOffset = 0.0f;


    Module(float, float);

    void cacheVelocity();
    void botToWheelVelocity(PolarCoordinates, float, Velocity);
    void velocityOptimiztion();

private:
    PolarCoordinates position;
};

float distanceBetween(float, float, float, float);

float angleBetween(float, float, float, float);

Velocity joystickToVelocity(float, float);

Velocity velocityAddition(const Velocity &, const Velocity &);

void normalizingSpeeds(Module &, Module &, Module &);

float ConstrainedAngle(float);
