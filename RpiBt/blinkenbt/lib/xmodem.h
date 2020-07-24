//
// Created by beton on 29.06.2020.
//

#ifndef BLINKENBT_XMODEM_H
#define BLINKENBT_XMODEM_H

#include <algorithm>
#include <iterator>
#include <cstdint>

struct xmData {
    uint8_t byte[128];
};

struct xmPacket {
    uint8_t header,
            packetNumber,
            reversePacketNumber;
    xmData  data;
    uint8_t checksum;
};

class xmodem {
public:
    const static uint8_t xmSOH = 0x01,
                         xmNAK = 0x15,
                         xmACK = 0x06,
                         xmEOT = 0x04;

    explicit xmodem(xmData input, uint8_t n);
    explicit xmodem(xmPacket packet);

    [[nodiscard]] uint8_t getHeader() const;
    xmData& getData();

    uint8_t genChecksum();
    bool checkAll();

private:
    xmPacket packet{};
};


#endif //BLINKENBT_XMODEM_H
