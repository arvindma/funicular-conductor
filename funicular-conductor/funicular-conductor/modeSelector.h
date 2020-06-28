#pragma once
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

bool isOneDigitIntBetween(const string str, int bottom, int top)
{

    int i = strtol(str.c_str(), NULL, 10);
    return str.length() == 1 && (bottom) <= i && i <= (top);
}

int askForMode(string modeOptions, int bottom, int top)
{
    cout << modeOptions;

    string modeIn;
    cin >> modeIn;

    while (isOneDigitIntBetween(modeIn, bottom, top) == false) {
        printf("Whoops... Enter a number between %d and %d\n", bottom, top);
        cin >> modeIn;
        if (isOneDigitIntBetween(modeIn, bottom, top)) {
            printf("What a save!\n");
        }
    }

    return strtol(modeIn.c_str(), NULL, 10);
}