#include <iostream>
#include <string>
#include <windows.h>
#include "comms.h"
#include "SerialPort/SerialPort.hpp"    //https://github.com/manashmandal/SerialPort
#include "PacketSerial/PacketSerial.h"  //https://github.com/bakercp/PacketSerial
#include "FastCRC/FastCRC.h"

using namespace std;
using namespace Radio;

void listComPorts();
void packetHandler(const void*, const uint8_t*, unsigned long);
void writeFloat(float, uint8_t*, size_t);
float readFloat(const uint8_t*, size_t);

boolean newDataAvailable = false;
ResponsePacket rxPacket;

unique_ptr<SerialPort> radioConnection;  //null by default
PacketSerial packetInterface;
FastCRC16 crc16;
FastCRC8 crc8;

void Radio::initialize()
{
    cout << "Enter number of the desired COM port, or ";

    while (1) {
        cout << "q to quit. Available ports :" << endl;
        listComPorts();
        string portInput;
        getline(cin, portInput);
        if (portInput.size() > 4)
            portInput = portInput.substr(0, 4);
        if (portInput == "q" || portInput == "Q")
            exit(0);

        portInput = "\\\\.\\COM" + portInput;

        const size_t nameSize = portInput.size() + 1;
        char* portName = new char[nameSize];
        portInput.copy(portName, nameSize);
        portName[nameSize - 1] = '\0';

        radioConnection = make_unique<SerialPort>(portName, CBR_115200);
        delete[] portName;

        if (radioConnection->isConnected()) {
            cout << endl << "Connection established at port " << portInput << endl << endl;
            Sleep(1000);
            break;
        }
        else {
            cout << endl << "Connection failed for " << portInput << endl << endl;
        }
    }

    packetInterface.setStream(radioConnection.get());
    packetInterface.setPacketHandler(&packetHandler);
}

void listComPorts()
{
    char lpTargetPath[5000]; // buffer to store the path

    for (int i = 0; i < 255; i++) // checking ports from COM0 to COM255
    {
        std::string str = "COM" + std::to_string(i); // converting to COM0, COM1, COM2
        DWORD retVal = QueryDosDeviceA(str.c_str(), lpTargetPath, 5000);

        if (retVal) //QueryDosDevice returns zero if it didn't find an object
        {
            std::cout << str << ": " << lpTargetPath << std::endl;
        }

        //if (GetLastError() == ERROR_INSUFFICIENT_BUFFER);
    }
}

void Radio::sendControlPacket(const Packet &packet)
{
    uint8_t buffer[PACKET_SIZE];

    buffer[PACKET_FLAGS_OFFSET] = (uint8_t)(packet.flags >> 8);
    buffer[PACKET_FLAGS_OFFSET + 1] = (uint8_t)packet.flags;
    writeFloat(packet.a1, buffer, PACKET_A1_OFFSET);
    writeFloat(packet.a2, buffer, PACKET_A2_OFFSET);
    writeFloat(packet.a3, buffer, PACKET_A3_OFFSET);
    writeFloat(packet.s1, buffer, PACKET_S1_OFFSET);
    writeFloat(packet.s2, buffer, PACKET_S2_OFFSET);
    writeFloat(packet.s3, buffer, PACKET_S3_OFFSET);

    uint16_t crc = crc16.ccitt(buffer, PACKET_SIZE - 2);
    buffer[PACKET_CRC_OFFSET] = crc >> 8;
    buffer[PACKET_CRC_OFFSET + 1] = (uint8_t)crc;

    packetInterface.send(buffer, PACKET_SIZE);
}

void Radio::update()
{
    packetInterface.update();
}

void packetHandler(const void* sender, const uint8_t* data, unsigned long size)
{
    if (size != RESPONSE_PACKET_SIZE) return;
    uint8_t crc = crc8.smbus(data, RESPONSE_PACKET_SIZE - 1);
    if (crc != data[RESPONSE_PACKET_CRC_OFFSET]) return;
    ResponsePacket temp;
    temp.flags = (data[RESPONSE_PACKET_FLAGS_OFFSET] << 8) + data[RESPONSE_PACKET_FLAGS_OFFSET + 1];
    temp.crc = crc;
    rxPacket = temp;
    newDataAvailable = true;
}

boolean Radio::packetAvailable()
{
    return newDataAvailable;
}

boolean Radio::ready()
{
    if (!radioConnection)
        return false;
    return radioConnection->isConnected();
}

Radio::ResponsePacket Radio::getLastPacket()
{
    newDataAvailable = false;
    return rxPacket;
}

#if __FLOAT_WORD_ORDER__ != __ORDER_LITTLE_ENDIAN__
#error incorrect byte order
#endif

//converts little-endian to big-endian
static void writeFloat(float value, uint8_t* data, size_t index)
{
    FloatUnion input;
    input.fp = value;

    data[index + 0] = input.bytes[3];
    data[index + 1] = input.bytes[2];
    data[index + 2] = input.bytes[1];
    data[index + 3] = input.bytes[0];
}

//converts big-endian to little-endian
static float readFloat(const uint8_t* data, size_t index)
{
    FloatUnion output;

    output.bytes[0] = data[index + 3];
    output.bytes[1] = data[index + 2];
    output.bytes[2] = data[index + 1];
    output.bytes[3] = data[index + 0];

    return output.fp;
}

MovingAverage::MovingAverage(uint8_t size_)
    : size(size_)
{
    if (size == 0)
        size = 1;
    data = new float[size];
}

MovingAverage::~MovingAverage()
{
    delete[] data;
}

void MovingAverage::add(float value)
{
    data[index] = value;
    index++;
    if (index > (size - 1))
        index = 0;
}

float MovingAverage::average()
{
    float output = 0;
    for (int_fast8_t i = 0; i < size; i++)
        output += data[i];
    return output / size;
}

void MovingAverage::zero()
{
    for (int_fast8_t i = 0; i < size; i++)
        data[i] = 0;
}