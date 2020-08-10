#pragma once
#include "botToModuleMath.h"

float distanceBetween(float r1, float theta1, float r2, float theta2) {
    return sqrt(r1 * r1 + r2 * r2 + 2 * r1 * r2 * cos(theta2 - theta1));
}

float angleBetween(float r1, float theta1, float r2, float theta2) {
    float angleOutput = theta1 - asin((r2 * sin(theta2 - theta1)) / (distanceBetween(r1, theta1, r2, theta2)));
    return angleOutput;
}

float angleRangeLimit(float angle) {
    if (angle < 0) {
        angle = fmod(angle, 2 * M_PI);
        angle += 2 * M_PI;
    }
    if (angle > 2 * M_PI) {
        angle = fmod(angle, 2 * M_PI);
    }
    return angle;
}

velocity joystickToVelocity(float normalizedJoystick, float angle) {
    velocity velocityOutput(normalizedJoystick, angle);
    return velocityOutput; //Add conversion to in/sec
}

velocity velocityAddition(velocity v1, velocity v2) {
    if (v1.speed != 0 && v2.speed != 0) {
        velocity velocityOutput(0.0, 0.0);
        velocityOutput.speed = distanceBetween(v1.speed, v1.angle, v2.speed, v2.angle);
        velocityOutput.angle = v1.angle + atan2f(v2.speed * sin(v2.angle - v1.angle), v1.speed + v2.speed * cos(v2.angle - v1.angle));
        return velocityOutput;
    }
    else if (v1.speed == 0 && v2.speed == 0)
        return velocity(0, 0);
    else if (v2.speed == 0)
        return v1;
    else
        return v2;
}

velocity botToWheelVelocity(polarCoordinates modulePosition, polarCoordinates rotationCenter, float rotationSpeed,
    velocity translationSpeed, float *normalizingConst) { // Velocities should be normalized from 0 to 1

    polarCoordinates deltacmp(0.0, 0.0); //Difference between center of rotation and module position (which is nothing when cR is (0,0))
    deltacmp.radius = distanceBetween(modulePosition.radius, modulePosition.angle, rotationCenter.radius, rotationCenter.angle);
    deltacmp.angle = angleBetween(modulePosition.radius, modulePosition.angle, rotationCenter.radius, rotationCenter.angle);

    velocity rotationVector(0.0, 0.0);
    rotationSpeed = rotationSpeed / MODULEP_RADIUS;
    rotationVector.speed = deltacmp.radius * abs(rotationSpeed);
    rotationVector.angle = deltacmp.angle + (M_PI / 2 * rotationSpeed / abs(rotationSpeed));

    velocity prenormalizedOutput = velocityAddition(rotationVector, translationSpeed);

    velocity velocityOutput(0, 0);
    if (prenormalizedOutput.speed > 1) {
        translationSpeed.speed = translationSpeed.speed / (translationSpeed.speed + rotationVector.speed);
        rotationVector.speed = rotationVector.speed / (translationSpeed.speed + rotationVector.speed);
        prenormalizedOutput = velocityAddition(rotationVector, translationSpeed);
    }
    if (prenormalizedOutput.speed > 1)
        *normalizingConst = prenormalizedOutput.speed;
    else
        *normalizingConst = 1.0;
    velocityOutput.speed = prenormalizedOutput.speed;
    velocityOutput.angle = prenormalizedOutput.angle;
    return velocityOutput;
}

void normalizingSpeeds(float s1, float s2, float s3, float n1, float n2, float n3, float *output) {
    float n;
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
    if (a >= (preA - M_PI / 2) && a <= (preA + M_PI / 2))
        return 1;
    return -1;
}

float angleDirectionOptimization(float a, int d) {
    if (d == 1)
        return a;
    else if (d == -1)
        return angleRangeLimit(a + M_PI);
}

