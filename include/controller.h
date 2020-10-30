#pragma once
#include <windows.h>
#include <Xinput.h>
#include <iostream>
#include <string>

#include "botConstants.h"

#define INPUT_DEADZONE 4000

enum class LorR {
    L,
    R
};

namespace Controller {
    bool controllerCheck();
    float joystickMagnitude(LorR);
    float joystickAngle(LorR);
    float triggersMagnitude();
    boolean getButton(WORD);
    WORD getButtons();
}