#pragma once
#include <windows.h>
#include <Xinput.h>
#include <iostream>
#include <string>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define INPUT_DEADZONE 4000

class controllerInfo {
public:
    bool controllerCheck() {
        XINPUT_STATE state;
        DWORD controllerConnected = XInputGetState(0, &state);

        if(controllerConnected != ERROR_SUCCESS)
        {
            printf("pLuG iN tHE cOntrOlLeR\n Press f if you don't have one\n");
            std::string f = "no";
            std::cin >> f;
            if (f == "f") return false;
            else 
            {
                DWORD controllerConnected = XInputGetState(0, &state);
                if (controllerConnected == ERROR_SUCCESS) return true;
            }
        }
        return true;
    }

    float joystickMagnitude(char LorR) {
        XINPUT_STATE state;
        DWORD controllerConnected = XInputGetState(0, &state);
        float X = 0;
        float Y = 0;
        float magnitude = 0;
        float normalizedMagnitude = 0;
        if (LorR == 'L') {
            X = state.Gamepad.sThumbLX;
            Y = state.Gamepad.sThumbLY;
        }
        else if (LorR == 'R') {
            X = state.Gamepad.sThumbRX;
            Y = state.Gamepad.sThumbRY;
        }
        else {
            printf("BRUHHHHHHHHH");
        }
        magnitude = sqrt(X * X + Y * Y);
        if (abs(magnitude) > INPUT_DEADZONE) {
            //clip the magnitude at its expected maximum value
            if (abs(magnitude) > 32767) magnitude = magnitude / abs(magnitude) * 32767;

            //adjust magnitude relative to the end of the dead zone
            magnitude = magnitude - magnitude / abs(magnitude) * INPUT_DEADZONE;

            //optionally normalize the magnitude with respect to its expected range
            //giving a magnitude value of 0.0 to 1.0
            normalizedMagnitude = magnitude / (32767 - INPUT_DEADZONE);
        }
        return normalizedMagnitude;
    }
   
    float joystickAngle(char LorR) {
        XINPUT_STATE state;
        DWORD controllerConnected = XInputGetState(0, &state);
        float X = 0;
        float Y = 0;
        if (LorR == 'L') {
            X = state.Gamepad.sThumbLX;
            Y = state.Gamepad.sThumbLY;
        }
        else if (LorR == 'R') {
            X = state.Gamepad.sThumbRX;
            Y = state.Gamepad.sThumbRY;
        }
        else {
            printf("BRUHHHHHHHHH");
        }
        float angle = 0;
        float magnitude = sqrt(X * X + Y * Y);
        if (abs(magnitude) < INPUT_DEADZONE) {
            return angle;
        }

        angle = atan(Y / X);

        if (X < 0) {

            angle = angle + M_PI;
            
        }
        else if (X > 0 && Y < 0) {

            angle = 2 * M_PI + angle;
            
        }
        return angle + M_PI/2;
        
    }

    float triggersMagnitude() {
        XINPUT_STATE state;
        DWORD controllerConnected = XInputGetState(0, &state);
        float lT = -state.Gamepad.bLeftTrigger/255.0;
        float rT = state.Gamepad.bRightTrigger/255.0;
        float triggers = lT + rT;
        return triggers;
    }
};