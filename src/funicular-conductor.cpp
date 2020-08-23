#pragma once
#include <string>
#include <sstream>
#include "controller.h"
#include "modeSelector.h"
#include "moduleMath.h"
#include "comms.h"

using namespace std;

//Declarations


int main() {
    string controlOptions = "1.Controller Mode\n2.Position Mode\n";
    int mode = askForMode(controlOptions,1,2);
    
    if (mode == 1) {//Controller Mode 
        printf("YAY! CONTROLLERS\n");

        Module module1(MODULEP_MAGNITUDE, MODULEP1_ANGLE),
            module2(MODULEP_MAGNITUDE, MODULEP2_ANGLE),
            module3(MODULEP_MAGNITUDE, MODULEP3_ANGLE);

        if (Controller::controllerCheck()) {
            while (Controller::controllerCheck()) {
                
                Velocity translationSpeed = joystickToVelocity(Controller::joystickMagnitude(LorR::L), Controller::joystickAngle(LorR::L));
                float rotationSpeed = Controller::triggersMagnitude();
                PolarCoordinates rotationCenter(MODULEP_MAGNITUDE * Controller::joystickMagnitude(LorR::R), Controller::joystickAngle(LorR::R)); //Center of rotation
                
                module1.botToWheelVelocity(rotationCenter, rotationSpeed, translationSpeed);
                module2.botToWheelVelocity(rotationCenter, rotationSpeed, translationSpeed);
                module3.botToWheelVelocity(rotationCenter, rotationSpeed, translationSpeed);

                normalizingSpeeds(module1, module2, module3);

                module1.velocityOptimiztion();
                module2.velocityOptimiztion();
                module3.velocityOptimiztion();

                module1.cacheVelocity();
                module2.cacheVelocity();
                module3.cacheVelocity();

                printf("Module 1 Angle and Speed: %f, %.0f\n", module1.velocity.magnitude, module1.velocity.angle * RAD_TO_DEG);
                printf("Module 2 Angle and Speed: %f, %.0f\n", module2.velocity.magnitude, module2.velocity.angle * RAD_TO_DEG);
                printf("Module 3 Angle and Speed: %f, %.0f\n", module3.velocity.magnitude, module3.velocity.angle * RAD_TO_DEG);
                Sleep(1000/2);
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