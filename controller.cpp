#include "controller.h"

bool Controller::controllerCheck() {
    XINPUT_STATE state;
    DWORD controllerConnected = XInputGetState(0, &state);

    if (controllerConnected != ERROR_SUCCESS)
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

float Controller::joystickMagnitude(LorR dir) {
    XINPUT_STATE state;
    DWORD controllerConnected = XInputGetState(0, &state);
    float X = 0;
    float Y = 0;
    float magnitude = 0;
    float normalizedMagnitude = 0;
    if (dir == LorR::L) {
        X = state.Gamepad.sThumbLX;
        Y = state.Gamepad.sThumbLY;
    }
    else if (dir == LorR::R) {
        X = state.Gamepad.sThumbRX;
        Y = state.Gamepad.sThumbRY;
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

float Controller::joystickAngle(LorR dir) {
    XINPUT_STATE state;
    DWORD controllerConnected = XInputGetState(0, &state);
    float X = 0;
    float Y = 0;
    if (dir == LorR::L) {
        X = state.Gamepad.sThumbLX;
        Y = state.Gamepad.sThumbLY;
    }
    else if (dir == LorR::R) {
        X = state.Gamepad.sThumbRX;
        Y = state.Gamepad.sThumbRY;
    }
    float angle = 0;
    float magnitude = sqrt(X * X + Y * Y);
    if (abs(magnitude) < INPUT_DEADZONE) {
        return angle;
    }

    angle = atan(Y / X);

    if (X < 0) {

        angle = angle + F_PI;

    }
    else if (X > 0 && Y < 0) {

        angle = 2 * F_PI + angle;

    }
    return angle + F_PI / 2;

}

float Controller::triggersMagnitude() {
    XINPUT_STATE state;
    DWORD controllerConnected = XInputGetState(0, &state);
    float lT = -state.Gamepad.bLeftTrigger / 255.0f;
    float rT = state.Gamepad.bRightTrigger / 255.0f;
    float triggers = lT + rT;
    return triggers;
}