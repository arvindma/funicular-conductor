#pragma once
#include <iostream>
#include <string>
#include "controller.h"
#include "modeSelector.h"
#include "moduleMath.h"
#include "comms.h"

using namespace std;

//int main() {
//    Radio::initialize();
//    int sent = 0;
//    int received = 0;
//
//    while (1)
//    {
//        Radio::Packet packet;
//        SETFLAG(packet.flags, Radio::FLAG_ACK);
//        Radio::sendControlPacket(packet);
//        sent++;
//        Sleep(1000 / 20);
//        Radio::update();
//        if (Radio::packetAvailable())
//        {
//            Radio::ResponsePacket rxPacket = Radio::getLastPacket();
//            received++;
//        }
//        cout << "sent: " << sent << " received: " << received << endl;
//    }
//}


int main() {
    string controlOptions = "1.Controller Mode\n2.Position Mode\n";
    int mode = askForMode(controlOptions,1,2);

    //Radio::initialize();
    
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
                unsigned int maxSpeed = 100;
                
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

                Radio::Packet packet;
                packet.a1 = module1.velocity.angle;
                packet.a2 = module2.velocity.angle;
                packet.a3 = module3.velocity.angle;
                packet.s1 = maxSpeed * module1.velocity.magnitude;
                packet.s2 = maxSpeed * module2.velocity.magnitude;
                packet.s3 = maxSpeed * module3.velocity.magnitude;

                if(Radio::ready())
                    Radio::sendControlPacket(packet);

                Sleep(1000/20);

                Radio::update();
                if (Radio::packetAvailable())
                {
                    Radio::ResponsePacket rxPacket = Radio::getLastPacket();
                }
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