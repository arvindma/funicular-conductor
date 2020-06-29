// funicular-conductor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#pragma once
#include <string>
#include <sstream>
#include "controller.h"
#include "modeSelector.h"
#include "botToModuleMath.h"

using namespace std;

//Declarations
int main();
bool isOneDigitIntBetween(string str, int bottom, int top);
int askForMode(string modeOptions, int bottom, int top);


int main() {
    string controlOptions = "1.Controller Mode\n2.Position Mode\n";
    int mode = askForMode(controlOptions,1,2);
    
    if (mode == 1) {//Controller Mode 
        printf("YAY! CONTROLLERS\n");

        controllerInfo controller1;
        const float moduleDistFromOrigin = 10.0;
        polarCoordinates module1p(moduleDistFromOrigin, M_PI / 2);
        polarCoordinates module2p(moduleDistFromOrigin, M_PI / 2 + M_PI * 2 / 3);
        polarCoordinates module3p(moduleDistFromOrigin, M_PI / 2 + M_PI * 4 / 3);

        if (controller1.controllerCheck()) {
            while (controller1.controllerCheck()) {
                
                velocity tS = velocity::joystickToVelocity(controller1.joystickMagnitude('L'), controller1.joystickAngle('L'));
                float rS = controller1.joystickMagnitude('R');/*Add radians per sec*/
                polarCoordinates cR(0, 0);
                printf("ROTATION SPEED: %f\n", rS);
                
                velocity module1v = velocity::botToWheelVelocity(module1p, cR, rS, tS);
                velocity module2v = velocity::botToWheelVelocity(module2p, cR, rS, tS);
                velocity module3v = velocity::botToWheelVelocity(module3p, cR, rS, tS);
                printf("Module 1 Angle and Speed: %f, %f\n", module1v.speed, (polarCoordinates::angleRangeLimit(module1v.angle)) / (2 * M_PI) * 360);
                printf("Module 2 Angle and Speed: %f, %f\n", module2v.speed, (polarCoordinates::angleRangeLimit(module2v.angle)) / (2 * M_PI) * 360);
                printf("Module 3 Angle and Speed: %f, %f\n", module3v.speed, (polarCoordinates::angleRangeLimit(module3v.angle)) / (2 * M_PI) * 360);
                Sleep(500);
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