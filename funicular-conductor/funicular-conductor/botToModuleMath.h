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

    polarCoordinates(float x, float y): radius(x), angle(y) {}

    static float distanceBetween(float r1, float theta1, float r2, float theta2) {
        return sqrt(r1 * r1 + r2 * r2 + 2 * r1 * r2 * cos(theta2 - theta1));
    }

    float angleBetween(float r1, float theta1, float r2, float theta2) {
        float angleOutput = theta1-asin((r2*sin(theta2-theta1))/(polarCoordinates::distanceBetween(r1, theta1, r2, theta2)));
        return angleOutput;
    }

    static float angleRangeLimit(float angle) {        //this should be improved
        while (angle < 0) {
            angle = angle + 2 * M_PI;
        }
        while (angle > 2*M_PI) {
            angle = angle - 2 * M_PI;
        }
        return angle;
    }
};

class velocity {
public:
    float speed;
    float angle;

    velocity(float x, float y) {
        speed = x;
        angle = y;
    }

    static velocity joystickToVelocity(float normalizedJoystick, float angle) {
        velocity velocityOutput(normalizedJoystick, angle);
        return velocityOutput; //Add conversion to in/sec
    }

    static velocity velocityAddition(velocity v1, velocity v2) {
        if (v1.speed != 0 && v2.speed != 0) {
            velocity velocityOutput(0.0, 0.0);
            velocityOutput.speed = polarCoordinates::distanceBetween(v1.speed, v1.angle, v2.speed, v2.angle);
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

    static velocity botToWheelVelocity(polarCoordinates mp, polarCoordinates cR, float rS, velocity tS, float n[]) { // Velocities should be normalized from 0 to 1
        polarCoordinates deltacmp(0.0, 0.0); //Difference between center of rotation and module position (which is nothing when cR is (0,0))
        deltacmp.radius = deltacmp.distanceBetween(mp.radius, mp.angle, cR.radius, cR.angle);
        deltacmp.angle = deltacmp.angleBetween(mp.radius, mp.angle, cR.radius, cR.angle);
        
        velocity rotationVector(0.0, 0.0);
        rS = rS / MODULEP_RADIUS;
        rotationVector.speed = deltacmp.radius * abs(rS);
        rotationVector.angle = deltacmp.angle + (M_PI / 2 * rS / abs(rS));
        
        velocity prenormalizedOutput = velocity::velocityAddition(rotationVector, tS);

        velocity velocityOutput(0, 0);
        if (prenormalizedOutput.speed > 1) {
            tS.speed = tS.speed / (tS.speed + rotationVector.speed);
            rotationVector.speed = rotationVector.speed / (tS.speed + rotationVector.speed);
            prenormalizedOutput = velocity::velocityAddition(rotationVector, tS);
        }
        if (prenormalizedOutput.speed > 1)
            n[0] = prenormalizedOutput.speed;
        else 
            n[0] = 1.0;
        velocityOutput.speed = prenormalizedOutput.speed;
        velocityOutput.angle = prenormalizedOutput.angle;
        return velocityOutput;
    }
    
    static void normalizingSpeeds(float s1, float s2, float s3, float n1, float n2, float n3, float s[]) {
        float n;
        if (n1 >= n2) {
            if (n1 >= n3) {
                n = n1;
            }
            else n = n3;
        }
        else if (n2 >= n3) 
            n = n2;
        s[0] = s1 / n;
        s[1] = s2 / n;
        s[2] = s3 / n;
    }

    static int speedDirectionOptimization(float preA, float a) {
        if (a >= (preA - M_PI / 2) && a <= (preA + M_PI / 2))
            return 1;
        return -1;
    }

    static float angleDirectionOptimization(float a, int d) {
        if (d == 1)
            return a;
        else if (d == -1)
            return polarCoordinates::angleRangeLimit(a + M_PI);
    }
};


