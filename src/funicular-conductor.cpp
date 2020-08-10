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

        float preSpeed[] = { 0.0,0.0,0.0 };
        float preAngle[] = { 0.0,0.0,0.0 };

        if (controller1.controllerCheck()) {
            while (controller1.controllerCheck()) {
                
                velocity translationSpeed = joystickToVelocity(controller1.joystickMagnitude('L'), controller1.joystickAngle('L'));
                float rotationSpeed = controller1.triggersMagnitude();
                polarCoordinates cR(MODULEP_RADIUS * controller1.joystickMagnitude('R'), controller1.joystickAngle('R')); //Center of rotation

                float addNormalizer1 = 1.0;
                float addNormalizer2 = 1.0;
                float addNormalizer3 = 1.0;

                velocity module1v = botToWheelVelocity(module1p, cR, rotationSpeed, translationSpeed, &addNormalizer1);
                velocity module2v = botToWheelVelocity(module2p, cR, rotationSpeed, translationSpeed, &addNormalizer2);
                velocity module3v = botToWheelVelocity(module3p, cR, rotationSpeed, translationSpeed, &addNormalizer3);

                float tempSpeeds[3];
                normalizingSpeeds(module1v.speed, module2v.speed, module3v.speed, addNormalizer1, addNormalizer2, addNormalizer3, &tempSpeeds[0]);
               
                module1v.angle = (angleRangeLimit(module1v.angle));
                module2v.angle = (angleRangeLimit(module2v.angle));
                module3v.angle = (angleRangeLimit(module3v.angle));

                int optimizationModule1 = speedDirectionOptimization(preAngle[0], module1v.angle);
                int optimizationModule2 = speedDirectionOptimization(preAngle[1], module2v.angle);
                int optimizationModule3 = speedDirectionOptimization(preAngle[2], module3v.angle);

                module1v.speed = -tempSpeeds[0] * optimizationModule1;
                module2v.speed = -tempSpeeds[1] * optimizationModule2;
                module3v.speed = -tempSpeeds[2] * optimizationModule3;
                module1v.angle = angleDirectionOptimization(module1v.angle, optimizationModule1);
                module2v.angle = angleDirectionOptimization(module2v.angle, optimizationModule2);
                module3v.angle = angleDirectionOptimization(module3v.angle, optimizationModule3);

                if (abs(module1v.speed) < 0.001) module1v.angle = preAngle[0];
                if (abs(module2v.speed) < 0.001) module2v.angle = preAngle[1];
                if (abs(module3v.speed) < 0.001) module3v.angle = preAngle[2];
                preSpeed[0] = module1v.speed;
                preSpeed[1] = module2v.speed;
                preSpeed[2] = module3v.speed;
                preAngle[0] = module1v.angle;
                preAngle[1] = module2v.angle;
                preAngle[2] = module3v.angle;

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