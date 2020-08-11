#pragma once
#include "botToModuleMath.h"

float distanceBetween(float r1, float theta1, float r2, float theta2) {
    return sqrt(r1 * r1 + r2 * r2 + 2 * r1 * r2 * cos(theta2 - theta1));
}

float angleBetween(float r1, float theta1, float r2, float theta2) {
    float angleOutput = theta1 - asin((r2 * sin(theta2 - theta1)) / (distanceBetween(r1, theta1, r2, theta2)));
    return angleOutput;
}

Velocity joystickToVelocity(float normalizedJoystick, float angle) {
    Velocity velocityOutput(normalizedJoystick, angle);
    return velocityOutput;
}

Velocity velocityAddition(Velocity *v1, Velocity *v2) {
    if (v1->magnitude != 0 && v2->magnitude != 0) {
        Velocity velocityOutput(0.0, 0.0);
        velocityOutput.magnitude = distanceBetween(v1->magnitude, v1->getAngle(), v2->magnitude, v2->getAngle());
        velocityOutput.setAngle(v1->getAngle() + atan2f(v2->magnitude * sin(v2->getAngle() - v1->getAngle()), v1->magnitude + v2->magnitude * cos(v2->getAngle() - v1->getAngle())));
        return velocityOutput;
    }
    else if (v1->magnitude == 0 && v2->magnitude == 0)
        return Velocity(0, 0);
    else if (v2->magnitude == 0)
        return *v1;
    else
        return *v2;
}

Velocity botToWheelVelocity(PolarCoordinates modulePosition, PolarCoordinates rotationCenter, float rotationSpeed,
    Velocity translationSpeed, float *normalizingConst) { // Velocities should be normalized from 0 to 1

    PolarCoordinates deltacmp(0.0, 0.0); //Difference between center of rotation and module position (which is nothing when cR is (0,0))
    deltacmp.magnitude = distanceBetween(modulePosition.magnitude, modulePosition.getAngle(), rotationCenter.magnitude, rotationCenter.getAngle());
    deltacmp.setAngle(angleBetween(modulePosition.magnitude, modulePosition.getAngle(), rotationCenter.magnitude, rotationCenter.getAngle()));

    Velocity rotationVector(0.0, 0.0);
    rotationSpeed = rotationSpeed / MODULEP_MAGNITUDE;
    rotationVector.magnitude = deltacmp.magnitude * abs(rotationSpeed);
    rotationVector.setAngle(deltacmp.getAngle() + (F_PI / 2 * rotationSpeed / abs(rotationSpeed)));

    Velocity prenormalizedOutput = velocityAddition(&rotationVector, &translationSpeed);

    Velocity velocityOutput(0, 0);
    if (prenormalizedOutput.magnitude > 1) {
        translationSpeed.magnitude = translationSpeed.magnitude / (translationSpeed.magnitude + rotationVector.magnitude);
        rotationVector.magnitude = rotationVector.magnitude / (translationSpeed.magnitude + rotationVector.magnitude);
        prenormalizedOutput = velocityAddition(&rotationVector, &translationSpeed);
    }
    if (prenormalizedOutput.magnitude > 1)
        *normalizingConst = prenormalizedOutput.magnitude;
    else
        *normalizingConst = 1.0;
    velocityOutput.magnitude = prenormalizedOutput.magnitude;
    velocityOutput.setAngle(prenormalizedOutput.getAngle());
    return velocityOutput;
}

void normalizingSpeeds(float s1, float s2, float s3, float n1, float n2, float n3, float *output) {
    float n = 1;
    if (n1 >= n2) {
        if (n1 >= n3) {
            n = n1;
        }
        else n = n3;
    }
    else if (n2 >= n3)
        n = n2;
    output[0] = s1 / n;
    output[1] = s2 / n;
    output[2] = s3 / n;
}

int speedDirectionOptimization(float preA, float a) {
    if (a >= (preA - F_PI / 2) && a <= (preA + F_PI / 2))
        return 1;
    return -1;
}

float angleDirectionOptimization(float a, int d) {
    if (d == -1)
        return a + F_PI;
    return a;
}

