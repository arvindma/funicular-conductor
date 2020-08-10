#pragma once
#include <string>
#include <sstream>
#include "controller.h"
#include "modeSelector.h"
#include "botToModuleMath.h"
#include "vector"
#if __FLOAT_WORD_ORDER__ != __ORDER_LITTLE_ENDIAN__
#error incorrect byte order
#endif

using namespace std;

//Declarations



int main() {
    string controlOptions = "1.Controller Mode\n2.Position Mode\n";
    int mode = askForMode(controlOptions,1,2);
    
    if (mode == 1) {//Controller Mode 
        printf("YAY! CONTROLLERS\n");

        controllerInfo controller1;
        polarCoordinates module1p(MODULEP_RADIUS, MODULEP1_ANGLE);
        polarCoordinates module2p(MODULEP_RADIUS, MODULEP2_ANGLE);
        polarCoordinates module3p(MODULEP_RADIUS, MODULEP3_ANGLE);

        vector<float> preS;
        vector<float> preA;
        preS = { 0.0,0.0,0.0 };
        preA = { 0.0,0.0,0.0 };
        if (controller1.controllerCheck()) {
            while (controller1.controllerCheck()) {
                
                velocity tS = velocity::joystickToVelocity(controller1.joystickMagnitude('L'), controller1.joystickAngle('L'));
                float rS = controller1.triggersMagnitude();
                polarCoordinates cR(MODULEP_RADIUS * controller1.joystickMagnitude('R'), controller1.joystickAngle('R')); //Center of rotation
                float addNormalizer1[1] = { 1.0 };
                float addNormalizer2[1] = { 1.0 };
                float addNormalizer3[1] = { 1.0 };
                float s[3];

                velocity module1v = velocity::botToWheelVelocity(module1p, cR, rS, tS, addNormalizer1);
                velocity module2v = velocity::botToWheelVelocity(module2p, cR, rS, tS, addNormalizer2);
                velocity module3v = velocity::botToWheelVelocity(module3p, cR, rS, tS, addNormalizer3);
                velocity::normalizingSpeeds(module1v.speed, module2v.speed, module3v.speed, addNormalizer1[0], addNormalizer2[0], addNormalizer3[0], s);
               
                module1v.angle = (polarCoordinates::angleRangeLimit(module1v.angle));
                module2v.angle = (polarCoordinates::angleRangeLimit(module2v.angle));
                module3v.angle = (polarCoordinates::angleRangeLimit(module3v.angle));

                int optimizationModule1 = velocity::speedDirectionOptimization(preA[0], module1v.angle);
                int optimizationModule2 = velocity::speedDirectionOptimization(preA[1], module2v.angle);
                int optimizationModule3 = velocity::speedDirectionOptimization(preA[2], module3v.angle);
                module1v.speed = -s[0] * optimizationModule1;
                module2v.speed = -s[1] * optimizationModule2;
                module3v.speed = -s[2] * optimizationModule3;
                module1v.angle = velocity::angleDirectionOptimization(module1v.angle, optimizationModule1);
                module2v.angle = velocity::angleDirectionOptimization(module2v.angle, optimizationModule2);
                module3v.angle = velocity::angleDirectionOptimization(module3v.angle, optimizationModule3);
                if (abs(module1v.speed) < 0.001) module1v.angle = preA[0];
                if (abs(module2v.speed) < 0.001) module2v.angle = preA[1];
                if (abs(module3v.speed) < 0.001) module3v.angle = preA[2];
                preS = {module1v.speed, module2v.speed, module3v.speed};
                preA = {module1v.angle, module2v.angle, module3v.angle};

                printf("Module 1 Angle and Speed: %f, %fπ\n", module1v.speed, module1v.angle / (M_PI));
                printf("Module 2 Angle and Speed: %f, %fπ\n", module2v.speed, module2v.angle / (M_PI));
                printf("Module 3 Angle and Speed: %f, %fπ\n", module3v.speed, module3v.angle / (M_PI));
                Sleep(1000/20);
            }
        }
    }
    
    if (mode == 2) { //Position Mode
        printf("YAY! POSITION\n");
    }

    printf("Press q to exit\n");
    string end;
    std::cin >> end;
    while (end != "q") {
        printf("That wasn't q...\n");
        std::cin >> end;
    }
}