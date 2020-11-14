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

float distanceBetween(float r1, float theta1, float r2, float theta2)
{
    return sqrt(r1 * r1 + r2 * r2 + 2 * r1 * r2 * cos(theta2 - theta1));
}

float angleBetween(float r1, float theta1, float r2, float theta2)
{
    float angleOutput = theta1 - asin((r2 * sin(theta2 - theta1)) / (distanceBetween(r1, theta1, r2, theta2)));
    return angleOutput;
}

Velocity joystickToVelocity(float normalizedJoystick, float angle)
{
    Velocity velocityOutput(normalizedJoystick, angle);
    return velocityOutput;
}

Velocity velocityAddition(const Velocity& v1, const Velocity& v2)
{
    if (v1.magnitude != 0 && v2.magnitude != 0) {
        Velocity velocityOutput(0.0, 0.0);
        velocityOutput.magnitude = distanceBetween(v1.magnitude, v1.getConstrainedAngle(), v2.magnitude, v2.getConstrainedAngle());
        velocityOutput.angle = (v1.getConstrainedAngle() + atan2f(v2.magnitude * sin(v2.getConstrainedAngle() - v1.getConstrainedAngle()), v1.magnitude + v2.magnitude * cos(v2.getConstrainedAngle() - v1.getConstrainedAngle())));
        return velocityOutput;
    }
    else if (v1.magnitude == 0 && v2.magnitude == 0)
        return Velocity(0, 0);
    else if (v2.magnitude == 0)
        return v1;
    else
        return v2;
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

    PolarCoordinates deltacmp(0.0, 0.0); //Difference between center of rotation and module position (which is nothing when cR is (0,0))
    deltacmp.magnitude = distanceBetween(position.magnitude, ConstrainedAngle(position.angle + angleOffset), rotationCenter.magnitude, rotationCenter.getConstrainedAngle());
    deltacmp.angle = (angleBetween(position.magnitude, ConstrainedAngle(position.angle + angleOffset), rotationCenter.magnitude, rotationCenter.getConstrainedAngle()));

    Velocity rotationVector(0.0, 0.0);
    rotationSpeed = rotationSpeed / MODULEP_MAGNITUDE;
    rotationVector.magnitude = deltacmp.magnitude * abs(rotationSpeed);
    rotationVector.angle = (deltacmp.getConstrainedAngle() - (F_PI / 2 * rotationSpeed / abs(rotationSpeed)));

    Velocity prenormalizedOutput = velocityAddition(rotationVector, translationSpeed);

    Velocity velocityOutput(0, 0);
    if (prenormalizedOutput.magnitude > 1) {
        translationSpeed.magnitude = translationSpeed.magnitude / (translationSpeed.magnitude + rotationVector.magnitude);
        rotationVector.magnitude = rotationVector.magnitude / (translationSpeed.magnitude + rotationVector.magnitude);
        prenormalizedOutput = velocityAddition(rotationVector, translationSpeed);
    }
    if (prenormalizedOutput.magnitude > 1)
        normalizingFactor = prenormalizedOutput.magnitude;
    else
        normalizingFactor = 1.0;
    velocityOutput.magnitude = prenormalizedOutput.magnitude;
    velocityOutput.angle = (prenormalizedOutput.getConstrainedAngle());
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
        float angularSpeed = angleDifference * 20.0 / (i + 1);
        
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