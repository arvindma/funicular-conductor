#pragma once
#include <windows.h>
#include <Xinput.h>
#include <iostream>
#include <string>

class controllerInfo {
    public: bool connectionCheck() {
        XINPUT_STATE state;
        DWORD controllerConnected = XInputGetState(0, &state);

        while (controllerConnected != ERROR_SUCCESS)
        {
            printf("pLuG iN tHE cOntrOlLeR\n");
            std::cin.get();
            DWORD controllerConnected = XInputGetState(0, &state);
            if (controllerConnected == ERROR_SUCCESS)
            {
                return true;
            }
        }
    }
    public: float JoystickMagnitude(char LorR) {
        XINPUT_STATE state;
        DWORD controllerConnected = XInputGetState(0, &state);
        float X = 0;
        float Y = 0;
        if (LorR == 'L')
        {
            X = state.Gamepad.sThumbLX;
            Y = state.Gamepad.sThumbLY;
        }
        else if (LorR == 'R')
        {
            X = state.Gamepad.sThumbRX;
            Y = state.Gamepad.sThumbRY;
        }
        float INPUT_DEADZONE = 3000;
        //determine how far the controller is pushed
        float magnitude = sqrt(X * X + Y * Y);

        float normalizedMagnitude = 0;

        //check if the controller is outside a circular dead zone
        if (magnitude > INPUT_DEADZONE)
        {
            //clip the magnitude at its expected maximum value
            if (magnitude > 32767) magnitude = 32767;

            //adjust magnitude relative to the end of the dead zone
            magnitude -= INPUT_DEADZONE;

            //optionally normalize the magnitude with respect to its expected range
            //giving a magnitude value of 0.0 to 1.0
            normalizedMagnitude = magnitude / (32767 - INPUT_DEADZONE);
        }
        else //if the controller is in the deadzone zero out the magnitude
        {
            magnitude = 0.0;
            normalizedMagnitude = 0.0;

        }
        return normalizedMagnitude;
    }
    public: float joystickAngle(char LorR) {
        XINPUT_STATE state;
        DWORD controllerConnected = XInputGetState(0, &state);
        float X = 0;
        float Y = 0;
        if (LorR == 'L')
        {
            X = state.Gamepad.sThumbLX;
            Y = state.Gamepad.sThumbLY;
        }
        else if (LorR == 'R')
        {
            X = state.Gamepad.sThumbRX;
            Y = state.Gamepad.sThumbRY;
        }
        float angle = atan(X / Y);

        if (X < 0)
        {

            angle = angle + 3.141592653f;
            
        }
        else if (X > 0 && Y < 0)
        {

            angle = 2 * 3.141592653f + angle;
            
        }
        return angle;
        
    }
};