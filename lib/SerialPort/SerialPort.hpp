/*
* Author: Manash Kumar Mandal
* Modified Library introduced in Arduino Playground which does not work
* This works perfectly
* LICENSE: MIT
*/

#pragma once

#include <windows.h>
#include <iostream>

//#define SERIALDEBUG

class SerialPort
{
private:
    HANDLE handler = 0;
    boolean connected = false;
    COMSTAT status = {};
    DWORD errors = 0;
public:
    explicit SerialPort();
    explicit SerialPort(const char*, const int);
    ~SerialPort();

    boolean connect(const char*, const int);
    void disconnect();
    boolean isConnected();

    int read(const uint8_t *buffer, DWORD buf_size);
    int read();
    boolean write(const uint8_t* buffer, DWORD buf_size);
    boolean write(uint8_t);
    int available();
};