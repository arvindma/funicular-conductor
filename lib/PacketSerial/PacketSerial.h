//
// Copyright (c) 2013 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier: MIT
//


#pragma once

#include "PacketSerial/Encoding/COBS.h"

/// \brief A template class enabling packet-based Serial communication.
///
/// Typically one of the typedefined versions are used, for example,
/// `COBSPacketSerial` or `SLIPPacketSerial`.
///
/// The template parameters allow the user to define their own packet encoder /
/// decoder, custom packet marker and receive buffer size.
///
/// \tparam EncoderType The static packet encoder class name.
/// \tparam PacketMarker The byte value used to mark the packet boundary.
/// \tparam BufferSize The number of bytes allocated for the receive buffer.
template<typename EncoderType, uint8_t PacketMarker = 0, unsigned long ReceiveBufferSize = 256>
class PacketSerial_
{
public:
    /// \brief A typedef describing the packet handler method.
    ///
    /// The packet handler method usually has the form:
    ///
    ///     void onPacketReceived(const uint8_t* buffer, unsigned long size);
    ///
    /// where buffer is a pointer to the incoming buffer array, and size is the
    /// number of bytes in the incoming buffer.
    typedef void (*PacketHandlerFunction)(const uint8_t* buffer, unsigned long size);

    /// \brief A typedef describing the packet handler method.
    ///
    /// The packet handler method usually has the form:
    ///
    ///     void onPacketReceived(void* sender, const uint8_t* buffer, unsigned long size);
    ///
    /// where sender is a pointer to the PacketSerial_ instance that recieved
    /// the buffer,  buffer is a pointer to the incoming buffer array, and size
    /// is the number of bytes in the incoming buffer.
    typedef void (*PacketHandlerFunctionWithSender)(const void* sender, const uint8_t* buffer, unsigned long size);

    /// \brief Construct a default PacketSerial_ device.
    PacketSerial_():
        _receiveBufferIndex(0),
        _stream(nullptr),
        _onPacketFunction(nullptr),
        _onPacketFunctionWithSender(nullptr)
    {
    }

    /// \brief Destroy the PacketSerial_ device.
    ~PacketSerial_()
    {
    }


    /// \brief Attach PacketSerial to an existing Arduino `Stream`.
    ///
    /// This `Stream` could be a standard `Serial` `Stream` with a non-default
    /// configuration such as:
    ///
    ///     PacketSerial myPacketSerial;
    ///
    ///     void setup()
    ///     {
    ///         Serial.begin(300, SERIAL_7N1);
    ///         myPacketSerial.setStream(&Serial);
    ///     }
    ///
    /// Or it might be a `SoftwareSerial` `Stream` such as:
    ///
    ///     PacketSerial myPacketSerial;
    ///     SoftwareSerial mySoftwareSerial(10, 11);
    ///
    ///     void setup()
    ///     {
    ///         mySoftwareSerial.begin(38400);
    ///         myPacketSerial.setStream(&mySoftwareSerial);
    ///     }
    ///
    /// Any class that implements the `Stream` interface should work, which
    /// includes some network objects.
    ///
    /// \param stream A pointer to an Arduino `Stream`.
    void setStream(SerialPort* stream)
    {
        _stream = stream;
    }

    /// \brief Get a pointer to the current stream.
    /// \warning Reading from or writing to the stream managed by PacketSerial_
    ///          may break the packet-serial protocol if not done so with care. 
    ///          Access to the stream is allowed because PacketSerial_ never
    ///          takes ownership of the stream and thus does not have exclusive
    ///          access to the stream anyway.
    /// \returns a non-const pointer to the stream, or nullptr if unset.
    SerialPort* getStream()
    {
        return _stream;
    }

    /// \brief Get a pointer to the current stream.
    /// \warning Reading from or writing to the stream managed by PacketSerial_
    ///          may break the packet-serial protocol if not done so with care. 
    ///          Access to the stream is allowed because PacketSerial_ never
    ///          takes ownership of the stream and thus does not have exclusive
    ///          access to the stream anyway.
    /// \returns a const pointer to the stream, or nullptr if unset.
    const SerialPort* getStream() const
    {
        return _stream;
    }

    /// \brief The update function services the serial connection.
    ///
    /// This must be called often, ideally once per `loop()`, e.g.:
    ///
    ///     void loop()
    ///     {
    ///         // Other program code.
    ///
    ///         myPacketSerial.update();
    ///     }
    ///
    void update()
    {
        if (_stream == nullptr) return;

        while (_stream->available() > 0)
        {
            uint8_t data = _stream->read();

            if (data == PacketMarker)
            {
                if (_onPacketFunction || _onPacketFunctionWithSender)
                {
                    uint8_t *_decodeBuffer = new uint8_t[_receiveBufferIndex];

                    unsigned long numDecoded = EncoderType::decode(_receiveBuffer,
                                                            _receiveBufferIndex,
                                                            _decodeBuffer);

                    if (_onPacketFunction)
                    {
                        _onPacketFunction(_decodeBuffer, numDecoded);
                    }
                    else if (_onPacketFunctionWithSender)
                    {
                        _onPacketFunctionWithSender(this, _decodeBuffer, numDecoded);
                    }
                    delete[] _decodeBuffer;
                }

                _receiveBufferIndex = 0;
                _recieveBufferOverflow = false;
            }
            else
            {
                if ((_receiveBufferIndex + 1) < ReceiveBufferSize)
                {
                    _receiveBuffer[_receiveBufferIndex++] = data;
                }
                else
                {
                    // The buffer will be in an overflowed state if we write
                    // so set a buffer overflowed flag.
                    _recieveBufferOverflow = true;
                }
            }
        }
    }

    /// \brief Set a packet of data.
    ///
    /// This function will encode and send an arbitrary packet of data. After
    /// sending, it will send the specified `PacketMarker` defined in the
    /// template parameters.
    ///
    ///     // Make an array.
    ///     uint8_t myPacket[2] = { 255, 10 };
    ///
    ///     // Send the array.
    ///     myPacketSerial.send(myPacket, 2);
    ///
    /// \param buffer A pointer to a data buffer.
    /// \param size The number of bytes in the data buffer.
    void send(const uint8_t* buffer, unsigned long size) const
    {
        if(_stream == nullptr || buffer == nullptr || size == 0) return;

        uint8_t *_encodeBuffer = new uint8_t[EncoderType::getEncodedBufferSize(size)];

        unsigned long numEncoded = EncoderType::encode(buffer,
                                                size,
                                                _encodeBuffer);

        _stream->write(_encodeBuffer, numEncoded);
        _stream->write(PacketMarker);
        
        delete[] _encodeBuffer;
    }

    /// \brief Set the function that will receive decoded packets.
    ///
    /// This function will be called when data is read from the serial stream
    /// connection and a packet is decoded. The decoded packet will be passed
    /// to the packet handler. The packet handler must have the form:
    ///
    /// The packet handler method usually has the form:
    ///
    ///     void onPacketReceived(const uint8_t* buffer, unsigned long size);
    ///
    /// The packet handler would then be registered like this:
    ///
    ///     myPacketSerial.setPacketHandler(&onPacketReceived);
    ///
    /// Setting a packet handler will remove all other packet handlers.
    ///
    /// \param onPacketFunction A pointer to the packet handler function.
    void setPacketHandler(PacketHandlerFunction onPacketFunction)
    {
        _onPacketFunction = onPacketFunction;
        _onPacketFunctionWithSender = nullptr;
    }

    /// \brief Set the function that will receive decoded packets.
    ///
    /// This function will be called when data is read from the serial stream
    /// connection and a packet is decoded. The decoded packet will be passed
    /// to the packet handler. The packet handler must have the form:
    ///
    /// The packet handler method usually has the form:
    ///
    ///     void onPacketReceived(const void* sender, const uint8_t* buffer, unsigned long size);
    ///
    /// To determine the sender, compare the pointer to the known possible
    /// PacketSerial senders.
    ///
    ///     void onPacketReceived(void* sender, const uint8_t* buffer, unsigned long size)
    ///     {
    ///         if (sender == &myPacketSerial)
    ///         {
    ///             // Do something with the packet from myPacketSerial.
    ///         }
    ///         else if (sender == &myOtherPacketSerial)
    ///         {
    ///             // Do something with the packet from myOtherPacketSerial.
    ///         }
    ///     }
    ///
    /// The packet handler would then be registered like this:
    ///
    ///     myPacketSerial.setPacketHandler(&onPacketReceived);
    ///
    /// Setting a packet handler will remove all other packet handlers.
    ///
    /// \param onPacketFunctionWithSender A pointer to the packet handler function.
    void setPacketHandler(PacketHandlerFunctionWithSender onPacketFunctionWithSender)
    {
        _onPacketFunction = nullptr;
        _onPacketFunctionWithSender = onPacketFunctionWithSender;
    }

    /// \brief Check to see if the receive buffer overflowed.
    ///
    /// This must be called often, directly after the `update()` function.
    ///
    ///     void loop()
    ///     {
    ///         // Other program code.
    ///         myPacketSerial.update();
    ///
    ///         // Check for a receive buffer overflow.
    ///         if (myPacketSerial.overflow())
    ///         {
    ///             // Send an alert via a pin (e.g. make an overflow LED) or return a
    ///             // user-defined packet to the sender.
    ///             //
    ///             // Ultimately you may need to just increase your recieve buffer via the
    ///             // template parameters.
    ///         }
    ///     }
    ///
    /// The state is reset every time a new packet marker is received NOT when 
    /// overflow() method is called.
    ///
    /// \returns true if the receive buffer overflowed.
    bool overflow() const
    {
        return _recieveBufferOverflow;
    }

private:
    PacketSerial_(const PacketSerial_&);
    PacketSerial_& operator = (const PacketSerial_&);

    bool _recieveBufferOverflow = false;

    uint8_t _receiveBuffer[ReceiveBufferSize] = { 0 };
    unsigned long _receiveBufferIndex = 0;

    SerialPort* _stream = nullptr;

    PacketHandlerFunction _onPacketFunction = nullptr;
    PacketHandlerFunctionWithSender _onPacketFunctionWithSender = nullptr;
};


/// \brief A typedef for the default COBS PacketSerial class.
typedef PacketSerial_<COBS> PacketSerial;

/// \brief A typedef for a PacketSerial type with COBS encoding.
typedef PacketSerial_<COBS> COBSPacketSerial;