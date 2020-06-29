#pragma once
#include <windows.h>
#include <Xinput.h>
#include <iostream>
#include <string>

class controllerInfo {
    public: bool controllerCheck() {
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
    public: float joystickMagnitude(char LorR) {
        XINPUT_STATE state;
        DWORD controllerConnected = XInputGetState(0, &state);
        float X = 0;
        float Y = 0;
        float magnitude = 0;
        float normalizedMagnitude = 0;
        float INPUT_DEADZONE = 4000;
        if (LorR == 'L') {
            X = state.Gamepad.sThumbLX;
            Y = state.Gamepad.sThumbLY;
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
            else {//if the controller is in the deadzone zero out the magnitude
                magnitude = 0.0;
                normalizedMagnitude = 0.0;
            }
        }
        else if (LorR == 'R') { //Rotation
            magnitude = state.Gamepad.sThumbRX;
            printf("x: %f\n", magnitude);
            
            if (abs(magnitude) > INPUT_DEADZONE) {
                //clip the magnitude at its expected maximum value
                if (abs(magnitude) > 32767) {
                    magnitude = magnitude / abs(magnitude) * 32767;
                }
                magnitude = magnitude - magnitude / abs(magnitude) * INPUT_DEADZONE;
                normalizedMagnitude = magnitude / (32767 - INPUT_DEADZONE);
                printf("rJoystick:  %f\n", normalizedMagnitude);
            }
            else normalizedMagnitude = 0;
        }
        return normalizedMagnitude;
    }
    public: float joystickAngle(char LorR) {
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
        float angle = atan(Y / X);

        if (X < 0) {

            angle = angle + 3.141592653f;
            
        }
        else if (X > 0 && Y < 0) {

            angle = 2 * 3.141592653f + angle;
            
        }
        return angle;
        
    }
};