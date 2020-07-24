//
// Created by beton on 24.06.2020.
//

#include "btcon.h"

btcon::btcon() {
    // allocate socket for bluetooth
    socketPort = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    // bind socket to port 1 of the first available bluetooth adapter
    localAddress.rc_family = AF_BLUETOOTH;
    localAddress.rc_bdaddr = (bdaddr_t){{0,0,0,0,0,0}};
    localAddress.rc_channel = 1;
    bind(socketPort, (sockaddr*)&localAddress, sizeof(localAddress));
}

btcon::~btcon() {
    close(socketPort);
}

btconState btcon::getState() const {
    return state;
}

void btcon::setState(const btconState stateNew) {
    state = stateNew;
}

bool btcon::listen() {
    ::listen(socketPort, 1);
    clientPort = accept(socketPort, (sockaddr*)&clientAddress, &clientAddressLen);
    return true;
}

bool btcon::handleConnection() {
    uint8_t thing = 0;
    setState(connected);
    do {
        int bytesRead = read(clientPort, &thing, sizeof(thing));
        if (state == exited) {
            break;
        }
        if (bytesRead > 0) {
            std::lock_guard lock(listLock);
            buffer.push_back(thing);
            bufferUpdate.notify_one();
        } else break;
    } while (true);
    close(clientPort);
    return true;
}

bool btcon::pickByteFromBuffer(uint8_t &byte) {
    std::lock_guard lock(listLock);
    if (buffer.empty())
        return false;

    byte = buffer.front();
    buffer.pop_front();
    return true;
}

bool btcon::sendByte(uint8_t byte) {
    return write(clientPort, &byte, 1) > 0;
}

void btcon::xmodemHandler() {
    bool xmWasHeader = false;
    int msgCounter;
    xmPacket packet{};
    while (true) {
        std::unique_lock lock(listLock);
        do {
            bufferUpdate.wait_for(lock, std::chrono::milliseconds(10));
            if (state == exited) return;
        } while (buffer.empty());
        uint8_t byte;
        while (!buffer.empty()) {
            byte = buffer.front();
            buffer.pop_front();
            if (xmWasHeader) {
                if (msgCounter > 2 && msgCounter < 131) {
                    packet.data.byte[msgCounter - 3] = byte;
                } else if (msgCounter == 1) {
                    packet.packetNumber = byte;
                } else if (msgCounter == 2) {
                    packet.reversePacketNumber = byte;
                } else if (msgCounter == 131) {
                    packet.checksum = byte;
                    xmodem x(packet);
                    if (x.checkAll()) {
                        sendByte(xmodem::xmACK);
                        std::lock_guard lock2(xmodemLock);
                        xmodemPackets.push_back(x);
                    } else {
                        sendByte(xmodem::xmNAK);
                        std::cout << "bad packet\n";
                    }
                    xmWasHeader = false;
                }
                msgCounter++;
            } else if (byte == xmodem::xmSOH) {
                std::cout << "start of header\n";
                packet = {};
                xmWasHeader = true;
                msgCounter = 1;
                packet.header = byte;
            } else if (byte == xmodem::xmACK) {
                std::cout << "end of sequence\n";
                packet = {};
                packet.header = xmodem::xmEOT;
                xmodemPackets.emplace_back(packet);
                xmodemUpdate.notify_one();
                sendByte(xmodem::xmACK);
            } else if (byte == xmodem::xmEOT) {
                std::cout << "end of transmission\n";
                state = exited;
                return;
            }
        }
    }
}

bool btcon::extractXmodemMessage(std::list<uint8_t> &list, bool clear = true) {
    if (clear) list.clear();
    bool complete;
    std::lock_guard lock(xmodemLock);
    if (xmodemPackets.empty()) return false;

    xmodem* a = nullptr;
    for (xmodem & pk : xmodemPackets) {
        if (pk.getHeader() == xmodem::xmEOT) {
            a = &pk;
            complete = true;
            break;
        }

        for (uint8_t & i : pk.getData().byte)
            list.push_back(i);
    }

    if (a == nullptr) {
        xmodemPackets.clear();
        complete = false;
        a = &xmodemPackets.back();
    }

    while (&xmodemPackets.front() != a) xmodemPackets.pop_front();
    if (!xmodemPackets.empty()) {
        if (xmodemPackets.front().getHeader() == xmodem::xmEOT) {
            xmodemPackets.pop_front();
        }
    }
    list.push_back(0);
    return complete;
}

void btcon::waitForXmodemUpdate(int timeout) {
    std::mutex m;
    std::unique_lock lock(m);
    xmodemUpdate.wait_for(lock, std::chrono::milliseconds(timeout));
}

void btcon::explicitClientClose() {
    close(clientPort);
}
