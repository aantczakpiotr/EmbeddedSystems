//
// Created by beton on 29.06.2020.
//

#include "xmodem.h"

xmodem::xmodem(xmData input, uint8_t n) {
    packet.header = xmSOH;
    packet.packetNumber = n;
    packet.reversePacketNumber = 255 - n;
    std::copy(std::begin(input.byte), std::end(input.byte), std::begin(packet.data.byte));
    packet.checksum = genChecksum();
}

xmodem::xmodem(xmPacket packet) {
    this->packet = packet;
}

uint8_t xmodem::genChecksum() {
    uint8_t sum = 0;
    for (uint8_t i : packet.data.byte) sum += i;
    return sum;
}

bool xmodem::checkAll() {
    return !(packet.checksum != genChecksum() ||
             packet.packetNumber + packet.reversePacketNumber != 255);
}

uint8_t xmodem::getHeader() const {
    return packet.header;
}

xmData& xmodem::getData() {
    return packet.data;
}

