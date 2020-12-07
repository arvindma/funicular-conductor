#include <iostream>
#include <cmath>
#include "botConstants.h"
#include "moduleMath.h"

Module::Module(float pcmag, float pcang)
    : position(pcmag, pcang)
{
}

void Module::cacheVelocity()
{
    for (int i = 0; i < 3 - 1; i++) {
        turns[i + 1] = turns[i];
        directionSwitchAngle[i + 1] = directionSwitchAngle[i];
        directionSwitch[i + 1] = directionSwitch[i];
        velocity[i + 1] = velocity[i];
    }
    
}

PolarCoordinates CartesianCoordinates::toPolar() {
    float angle;
    if (x == 0.0f && y == 0.0f)
        angle = 0.0f;
    else
        angle = ConstrainedAngle(atan2(y, x));
    PolarCoordinates returnCoord(sqrt(x * x + y * y), angle);
    return returnCoord;
}

CartesianCoordinates PolarCoordinates::toCartesian() {
    CartesianCoordinates returnCoord(magnitude * cos(angle), magnitude * sin(angle));
    return returnCoord;
}

Velocity PolarCoordinates::toVelocity() {
    Velocity x(magnitude, angle);
    return x;
}


PolarCoordinates polarAddition(PolarCoordinates coord1, PolarCoordinates coord2)
{   
  
    CartesianCoordinates temp1 = coord1.toCartesian();
    CartesianCoordinates temp2 = coord2.toCartesian();

    CartesianCoordinates returnCart(temp1.x + temp2.x, temp1.y + temp2.y);

    return returnCart.toPolar();

    
    
}


Velocity joystickToVelocity(float normalizedJoystick, float angle)
{
    Velocity velocityOutput(normalizedJoystick, angle);
    return velocityOutput;
}

float ConstrainedAngle(float angle)
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


void Module::botToWheelVelocity(PolarCoordinates rotationCenter, float rotationSpeed, Velocity translationSpeed)
{ // Velocities should be normalized from 0 to 1
    PolarCoordinates deltacmp = polarAddition(position, -rotationCenter);

    float k = 1 / (2*MODULEP_MAGNITUDE);

    Velocity rotationVector(deltacmp.magnitude * abs(rotationSpeed) * k, deltacmp.getConstrainedAngle() - (F_PI / 2 * rotationSpeed / abs(rotationSpeed)));
    if (rotationSpeed == 0.0f) {
        rotationVector.angle = 0.0f;
    }
    
    Velocity velocityOutput = polarAddition(translationSpeed, rotationVector).toVelocity();

    if (velocityOutput.magnitude > 1)
        normalizingFactor = velocityOutput.magnitude;
    else
        normalizingFactor = 1.0;

    velocity[0] = velocityOutput;
}

void normalizingSpeeds(Module& mod1, Module& mod2, Module& mod3)
{
    float n = mod1.normalizingFactor;
    if (n < mod2.normalizingFactor)
        n = mod2.normalizingFactor;
    if (n < mod3.normalizingFactor)
        n = mod3.normalizingFactor;
    if (n < 1.0f)
        n = 1.0f;
    mod1.velocity[0].magnitude /= n;
    mod2.velocity[0].magnitude /= n;
    mod3.velocity[0].magnitude /= n;
}

void Module::velocityOptimiztion()
{
    float preOptimizedAngle[2];
    for (int i = 0; i < 2; i++) {
        preOptimizedAngle[i] = velocity[i+1].angle - (directionSwitchAngle[i+1] + turns[i+1] * 2) * F_PI;
    }
    
    if (abs(velocity[0].magnitude) < 0.01)
    {
        velocity[0].angle = preOptimizedAngle[0];
        velocity[0].magnitude = 0;
    }
    int preturns = turns[1];
    if (abs(velocity[0].magnitude) > 0)
    {
        if (velocity[0].angle < preOptimizedAngle[0] - (F_PI + 0.0349066))
            turns[0] = turns[1] + 1;
        if (velocity[0].angle > preOptimizedAngle[0] + (F_PI + 0.0349066))
            turns[0] = turns[1] - 1;
    }
    for (int i = 0; i < 2; i++) {
        float angleDifference = abs(velocity[0].angle - preOptimizedAngle[i]);
        if (angleDifference > F_PI)
            angleDifference = abs(2 * F_PI - angleDifference);
        float angularSpeed = angleDifference * 20.0f / (i + 1);
        
        if (angularSpeed > MAX_ANGULAR_SPEED) {
            if (angleDifference > F_PI / 2) {
                if (velocity[0].angle > preOptimizedAngle[i]) {
                    directionSwitchAngle[0] = directionSwitchAngle[1] - 1;
                }
                else {
                    directionSwitchAngle[0] = directionSwitchAngle[1] + 1;
                }

                directionSwitch[0] = directionSwitch[1] * -1;

                if (preturns > turns[0])
                    turns[0]++;
                else if (preturns < turns[0])
                    turns[0]--;
                break;
            }
        }
        
    }
    velocity[0].magnitude *= directionSwitch[0];
    velocity[0].angle += (directionSwitchAngle[0] + turns[0] * 2) * F_PI;
}