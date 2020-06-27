// funicular-conductor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <sstream>
#include <windows.h>
#include <Xinput.h>

using namespace std;
//Declarations
bool isOneDigitIntBetween(string str, int bottom, int top);
int askForMode(string modeOptions, int bottom, int top);

int main()
{
    string controlOptions = "1.Controller Mode\n2.Position Mode\n";
    int mode = askForMode(controlOptions,1,2);
    
    if (mode == 1) //Controller Mode
    {
        printf("YAY! CONTROLLERS\n");
        XINPUT_STATE state;
        DWORD controllerConnected = XInputGetState(0, &state);

        while(controllerConnected != ERROR_SEVERITY_SUCCESS)
        {
            printf("pLuG iN tHE cOntrOlLeR, and restart the prgm becuz arvind dont no how 2 code\n");
            cin.get();
            DWORD controllerConnected = XInputGetState(0, &state);
        }
        printf("YAY\n");
        while (1)
        {
            XINPUT_STATE state;
            DWORD controllerConnected = XInputGetState(0, &state);
            float INPUT_DEADZONE = 1000;
            float LX = state.Gamepad.sThumbLX;
            float LY = state.Gamepad.sThumbLY;

            //determine how far the controller is pushed
            float magnitude = sqrt(LX * LX + LY * LY);

            //determine the direction the controller is pushed
            

            float normalizedMagnitude = 0;
            float angle = tan(LY / LX);
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
            cout << magnitude << endl;
            cout << normalizedMagnitude << endl;
            cout << "ANGLE:" << angle/(2*3.1415926535)*360 << endl;
            Sleep(500);

            if (normalizedMagnitude > 0.99)
            {
                break;
            }
        }
        

    }
    
    if (mode == 2) //Position Mode
    {
        printf("YAY! POSITION\n");
    }

    printf("Press q to exit\n");
    string end;
    cin >> end;
    while (end != "q") {
        printf("That wasn't q...\n");
        cin >> end;
    }
}

bool isOneDigitIntBetween(const string str,int bottom, int top)
{
    
    int i = strtol(str.c_str(),NULL,10);
    return str.length() == 1 && (bottom) <= i && i <= (top);    
}

int askForMode(string modeOptions, int bottom, int top)
{
    cout << modeOptions;

    string modeIn;
    cin >> modeIn;

    while (isOneDigitIntBetween(modeIn, bottom, top) == false)
    {
        printf("Whoops... Enter a number between %d and %d\n",bottom,top);
        cin >> modeIn;
        if (isOneDigitIntBetween(modeIn, bottom, top))
        {
            printf("What a save!\n");
        }
    }

    return strtol(modeIn.c_str(),NULL,10);
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
