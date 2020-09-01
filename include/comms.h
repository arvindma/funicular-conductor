#pragma once

#include "radio_defs.h"
#include <inttypes.h>

namespace Radio
{
    void initialize();
    void sendControlPacket(const Packet &);
    void update();
    boolean ready();
    boolean packetAvailable();
    ResponsePacket getLastPacket();

    class MovingAverage
    {
    public:
        MovingAverage(uint8_t);
        ~MovingAverage();
        void add(float);
        float average(void);
        void zero(void);

    private:
        uint8_t size;
        uint8_t index = 0;
        float* data;
    };
} // namespace RADIO