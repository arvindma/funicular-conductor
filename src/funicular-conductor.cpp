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

        PolarCoordinates module1p(MODULEP_MAGNITUDE, MODULEP1_ANGLE);
        PolarCoordinates module2p(MODULEP_MAGNITUDE, MODULEP2_ANGLE);
        PolarCoordinates module3p(MODULEP_MAGNITUDE, MODULEP3_ANGLE);

        float preSpeed[] = { 0.0,0.0,0.0 };
        float preAngle[] = { 0.0,0.0,0.0 };

        if (Controller::controllerCheck()) {
            while (Controller::controllerCheck()) {
                
                Velocity translationSpeed = joystickToVelocity(Controller::joystickMagnitude(LorR::L), Controller::joystickAngle(LorR::L));
                float rotationSpeed = Controller::triggersMagnitude();
                PolarCoordinates rotationCenter(MODULEP_MAGNITUDE * Controller::joystickMagnitude(LorR::R), Controller::joystickAngle(LorR::R)); //Center of rotation

                float addNormalizer1 = 1.0;
                float addNormalizer2 = 1.0;
                float addNormalizer3 = 1.0;

                Velocity module1v = botToWheelVelocity(module1p, rotationCenter, rotationSpeed, translationSpeed, &addNormalizer1);
                Velocity module2v = botToWheelVelocity(module2p, rotationCenter, rotationSpeed, translationSpeed, &addNormalizer2);
                Velocity module3v = botToWheelVelocity(module3p, rotationCenter, rotationSpeed, translationSpeed, &addNormalizer3);

                float tempSpeeds[3];
                normalizingSpeeds(module1v.magnitude, module2v.magnitude, module3v.magnitude, addNormalizer1, addNormalizer2, addNormalizer3, &tempSpeeds[0]);

                int optimizationModule1 = speedDirectionOptimization(preAngle[0], module1v.getAngle());
                int optimizationModule2 = speedDirectionOptimization(preAngle[1], module2v.getAngle());
                int optimizationModule3 = speedDirectionOptimization(preAngle[2], module3v.getAngle());

                module1v.magnitude = -tempSpeeds[0] * optimizationModule1;
                module2v.magnitude = -tempSpeeds[1] * optimizationModule2;
                module3v.magnitude = -tempSpeeds[2] * optimizationModule3;
                module1v.setAngle(angleDirectionOptimization(module1v.getAngle(), optimizationModule1));
                module2v.setAngle(angleDirectionOptimization(module2v.getAngle(), optimizationModule2));
                module3v.setAngle(angleDirectionOptimization(module3v.getAngle(), optimizationModule3));

                if (abs(module1v.magnitude) < 0.001)
                    module1v.setAngle(preAngle[0]);
                if (abs(module2v.magnitude) < 0.001)
                    module2v.setAngle(preAngle[1]);
                if (abs(module3v.magnitude) < 0.001)
                    module3v.setAngle(preAngle[2]);

                preSpeed[0] = module1v.magnitude;
                preSpeed[1] = module2v.magnitude;
                preSpeed[2] = module3v.magnitude;
                preAngle[0] = module1v.getAngle();
                preAngle[1] = module2v.getAngle();
                preAngle[2] = module3v.getAngle();

                printf("Module 1 Angle and Speed: %f, %f\n", module1v.magnitude, 360 * module1v.getAngle() / (2 * F_PI));
                printf("Module 2 Angle and Speed: %f, %f\n", module2v.magnitude, 360 * module2v.getAngle() / (2 * F_PI));
                printf("Module 3 Angle and Speed: %f, %f\n", module3v.magnitude, 360 * module3v.getAngle() / (2 * F_PI));
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