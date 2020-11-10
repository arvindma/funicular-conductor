﻿#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <Xinput.h>
#include "controller.h"
#include "moduleMath.h"
#include "comms.h"

using namespace std;
void accurateDelay(unsigned long);

//int main() {
//    Radio::initialize();
//    int sent = 0;
//    int received = 0;
//
//    LARGE_INTEGER time, lastTime, oneSec;
//    QueryPerformanceCounter(&lastTime);
//    QueryPerformanceFrequency(&oneSec);
//
//    while (1)
//    {
//        Radio::Packet packet;
//        SETFLAG(packet.flags, Radio::FLAG_ACK);
//        Radio::sendControlPacket(packet);
//        sent++;
//        accurateDelay(1000 / 50);
//        Radio::update();
//        if (Radio::packetAvailable())
//        {
//            Radio::ResponsePacket rxPacket = Radio::getLastPacket();
//            received++;
//        }
//
//        QueryPerformanceCounter(&time);
//        if (time.QuadPart - lastTime.QuadPart > oneSec.QuadPart) {
//            QueryPerformanceCounter(&lastTime);
//            cout << (float)(100 * received / sent) << "%  " << sent << endl;
//            received = 0;
//            sent = 0;
//            //cout << "sent: " << sent << " received: " << received << " lost: " << sent - received << endl;
//        }
//    }
//}

void accurateDelay(unsigned long ms)
{
    if (ms == 0)
        return;
    LARGE_INTEGER now, start, second;
    QueryPerformanceFrequency(&second);
    LONGLONG counts = (LONGLONG)(second.QuadPart * ((double)ms / 1000.0f));
    QueryPerformanceCounter(&start);
    while (1)
    {
        QueryPerformanceCounter(&now);
        if (now.QuadPart - start.QuadPart > counts)
            break;
    }
}

void clear() {
    COORD topLeft = { 0, 0 };
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    GetConsoleScreenBufferInfo(console, &screen);
    FillConsoleOutputCharacterA(
        console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    FillConsoleOutputAttribute(
        console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
        screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    SetConsoleCursorPosition(console, topLeft);
}



int main() {
    cout << "1 -> Controller Mode\n2 -> Position Mode\n";
    int mode = 1;

    while (1)
    {
        string modeIn;
        std::getline(cin, modeIn);

        try
        {
            mode = stoi(modeIn);
        }
        catch (exception ex)
        {
            cout << "Noooo!";
        }
        if (mode > 0 && mode < 3)
            break;
        cout << "Whoops..." << endl;
    }


    //Radio::initialize();

    if (mode == 1) {//Controller Mode 
        printf("YAY! CONTROLLERS\n");

        Module module1(MODULEP_MAGNITUDE, MODULEP1_ANGLE),
            module2(MODULEP_MAGNITUDE, MODULEP2_ANGLE),
            module3(MODULEP_MAGNITUDE, MODULEP3_ANGLE);

        float botAngle = 0.0f;

        if (Controller::controllerCheck()) {
            while (true) {

                Velocity translationSpeed = joystickToVelocity(Controller::joystickMagnitude(LorR::L), Controller::joystickAngle(LorR::L));
                float rotationSpeed = Controller::triggersMagnitude();
                PolarCoordinates rotationCenter(MODULEP_MAGNITUDE * Controller::joystickMagnitude(LorR::R), Controller::joystickAngle(LorR::R)); //Center of rotation

                unsigned int maxSpeed = 120;

                module1.position.angle += botAngle;
                module2.position.angle += botAngle;
                module3.position.angle += botAngle;

                module1.botToWheelVelocity(rotationCenter, rotationSpeed, translationSpeed);
                module2.botToWheelVelocity(rotationCenter, rotationSpeed, translationSpeed);
                module3.botToWheelVelocity(rotationCenter, rotationSpeed, translationSpeed);

                normalizingSpeeds(module1, module2, module3);
                
                module1.velocityOptimiztion();
                module2.velocityOptimiztion();
                module3.velocityOptimiztion();

                clear();
                printf("Module 1 Angle and Speed and turns: %f, %.0f, %i\n", module1.velocity[0].magnitude, module1.velocity[0].angle * RAD_TO_DEG, module1.turns[1]);
                printf("Module 2 Angle and Speed and turns: %f, %.0f, %i\n", module2.velocity[0].magnitude, module2.velocity[0].angle * RAD_TO_DEG, module2.turns[1]);
                printf("Module 3 Angle and Speed and turns: %f, %.0f, %i\n", module3.velocity[0].magnitude, module3.velocity[0].angle * RAD_TO_DEG, module3.turns[1]);
                
                Radio::Packet packet = {};
                SETFLAG(packet.flags, Radio::FLAG_ENABLE);
                packet.a1 = module1.velocity[0].angle;
                packet.a2 = module2.velocity[0].angle;
                packet.a3 = module3.velocity[0].angle;
                packet.s1 = maxSpeed * module1.velocity[0].magnitude;
                packet.s2 = maxSpeed * module2.velocity[0].magnitude;
                packet.s3 = maxSpeed * module3.velocity[0].magnitude;

                module1.cacheVelocity();
                module2.cacheVelocity();
                module3.cacheVelocity();

                if (Controller::getButton(XINPUT_GAMEPAD_B))
                    CLRFLAG(packet.flags, Radio::FLAG_ENABLE);

                /*if (Radio::ready())
                    Radio::sendControlPacket(packet);*/

                accurateDelay(1000 / 20);

                /*Radio::update();
                if (Radio::packetAvailable())
                {
                    Radio::ResponsePacket rxPacket = Radio::getLastPacket();

                    if (GETFLAG(rxPacket.flags, Radio::RESPONSE_FLAG_BUSY))
                    {
                        system("cls");
                        printf("waiting for robot");
                        for (int i = 0; i < 9; i++)
                        {
                            printf(".");
                            accurateDelay(1000);
                        }
                        printf("\n");
                    }
                }*/

                if (Controller::getButton(XINPUT_GAMEPAD_B))
                {
                    accurateDelay(1000 / 20);
                    exit(1);
                }
            }
        }
    }

    if (mode == 2) { //Position Mode
        printf("YAY! POSITION\n");
    }

    printf("Press q to exit\n");
    string end;
    std::getline(cin, end);
    while (end != "q") {
        printf("That wasn't q...\n");
        std::getline(cin, end);
    }
}