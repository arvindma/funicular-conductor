/*
* Author: Manash Kumar Mandal
* Modified Library introduced in Arduino Playground which does not work
* This works perfectly
* LICENSE: MIT
*/

#include "SerialPort/SerialPort.hpp"

SerialPort::SerialPort() {}

SerialPort::SerialPort(const char *portName, const int baudRate)
{
    connect(portName, baudRate);
}

SerialPort::~SerialPort()
{
    disconnect();
}

boolean SerialPort::connect(const char* portName, const int baudRate)
{
    if (isConnected())
        disconnect();
    handler = CreateFileA(static_cast<LPCSTR>(portName),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (handler == INVALID_HANDLE_VALUE)
    {
#ifdef SERIALDEBUG

        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            std::cerr << "ERROR: Handle was not attached. Reason: " << portName << " not available\n";
        }
        else
        {
            std::cerr << "ERROR: unknown\n";
        }
#endif // SERIALDEBUG
        return false;
    }
    else
    {
        DCB dcbSerialParameters = { 0 };

        if (!GetCommState(handler, &dcbSerialParameters))
        {
#ifdef SERIALDEBUG
            std::cerr << "Failed to get current serial parameters\n";
#endif // SERIALDEBUG
            return false;
        }
        else
        {
            dcbSerialParameters.BaudRate = baudRate;
            dcbSerialParameters.ByteSize = 8;
            dcbSerialParameters.StopBits = ONESTOPBIT;
            dcbSerialParameters.Parity = NOPARITY;
            dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

            if (!SetCommState(handler, &dcbSerialParameters))
            {
#ifdef SERIALDEBUG
                std::cerr << "ALERT: could not set serial port parameters\n";
#endif // SERIALDEBUG
                return false;
            }
            else
            {
                connected = true;
                PurgeComm(handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
                return true;
            }
        }
    }
}

void SerialPort::disconnect()
{
    if (isConnected())  //update connected status
    {
        CloseHandle(handler);
    }
    connected = false;
}

boolean SerialPort::isConnected()
{
    return connected;
}

// Reading bytes from serial port to buffer;
// returns read bytes count, or if error occurs, returns 0
int SerialPort::read(const uint8_t *buffer, DWORD buf_size)
{
    if (!connected)
        return -1;
    DWORD bytesRead{};
    DWORD toRead = 0;

    ClearCommError(handler, &errors, &status);

    if (status.cbInQue > 0)
    {
        if (status.cbInQue > buf_size)
        {
            toRead = buf_size;
        }
        else
        {
            toRead = status.cbInQue;
        }
    }

    memset((void*) buffer, 0, buf_size);

    if (ReadFile(handler, (void*) buffer, toRead, &bytesRead, nullptr))
    {
        return bytesRead;
    }

    return -1;
}

int SerialPort::read()
{
    if (!connected)
        return -1;
    if (available()) {
        uint8_t value;
        read(&value, 1);
        return (int)value;
    }
    else
        return -1;
}

// Sending provided buffer to serial port;
// returns true if succeed, false if not
boolean SerialPort::write(const uint8_t *buffer, DWORD buf_size)
{
    if (!connected)
        return false;
    DWORD bytesSend;

    if (!WriteFile(handler, (void*) buffer, buf_size, &bytesSend, 0))
    {
        ClearCommError(handler, &errors, &status);
        return false;
    }
    
    return true;
}


boolean SerialPort::write(uint8_t value)
{
    if (!connected)
        return false;
    return write(&value, 1);
}

int SerialPort::available()
{
    if (!connected)
        return 0;

    DWORD bytesRead{};
    unsigned int toRead = 0;

    ClearCommError(handler, &errors, &status);
    return status.cbInQue;
}