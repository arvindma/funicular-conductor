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
    previousVelocity = velocity;
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
    deltacmp.magnitude = distanceBetween(position.magnitude, position.getConstrainedAngle(), rotationCenter.magnitude, rotationCenter.getConstrainedAngle());
    deltacmp.angle = (angleBetween(position.magnitude, position.getConstrainedAngle(), rotationCenter.magnitude, rotationCenter.getConstrainedAngle()));

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
    velocity = velocityOutput;
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
    mod1.velocity.magnitude /= n;
    mod2.velocity.magnitude /= n;
    mod3.velocity.magnitude /= n;
}
void Module::totalAngle() {
    velocity.angle += (directionSwitchAngle) * F_PI;
    if (abs(velocity.magnitude) > 0) 
    {
        if (ConstrainedAngle(velocity.angle + directionSwitchAngle % 2 * F_PI) < 
            ConstrainedAngle(previousVelocity.angle + directionSwitchAngle % 2 * F_PI) - F_PI)
            turns++;
        if (ConstrainedAngle(velocity.angle + directionSwitchAngle % 2 * F_PI) > 
            ConstrainedAngle(previousVelocity.angle + directionSwitchAngle % 2 * F_PI) + F_PI)
            turns--;
    }
    velocity.angle += (turns * 2) * F_PI;
}
void Module::velocityOptimiztion()
{
    
    if (abs(velocity.magnitude) < 0.01)
    {
        velocity.angle = (previousVelocity.angle);
        velocity.magnitude = 0;
    }
    float angleDifference = abs(ConstrainedAngle(velocity.angle + directionSwitchAngle * F_PI) - previousVelocity.getConstrainedAngle());
    if (angleDifference > F_PI)
        angleDifference = abs(2 * F_PI - angleDifference);
    
    if (angleDifference > F_PI / 2) {
        if (velocity.getConstrainedAngle() > previousVelocity.getConstrainedAngle()) {
            directionSwitchAngle--;
        }
        else {
            directionSwitchAngle++;
        }
            directionSwitch *= -1;
    }
    
    velocity.magnitude *= directionSwitch;
    totalAngle();
    if (abs(velocity.magnitude) < 0.01)
    {
        velocity.angle = (previousVelocity.angle);
        velocity.magnitude = 0;
    }
}