#pragma once

#include "radio_defs.h"

namespace Radio
{
    void initialize();
    void sendControlPacket(const Packet &);
    void update();
    boolean ready();
    boolean packetAvailable();
    ResponsePacket getLastPacket();
} // namespace RADIO