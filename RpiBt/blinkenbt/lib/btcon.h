//
// Created by beton on 24.06.2020.
//

#ifndef PI2_BTCON_H
#define PI2_BTCON_H
// lib
#include "xmodem.h"

// unix
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <sys/socket.h>
#include <unistd.h>

// std library
#include <atomic>
#include <list>
#include <mutex>
#include <condition_variable>
#include <iostream>

enum btconState {
    initial,
    listeningWaiting,
    listenedAccepted,
    listenedDenied,
    connected,
    exited
};

class btcon {
public:

    // constructors
    btcon();

    // destructors
    ~btcon();

    // methods
    bool listen();
    bool handleConnection();
    bool pickByteFromBuffer(uint8_t &byte);
    bool sendByte(uint8_t byte);
    bool extractXmodemMessage(std::list<uint8_t>& list, bool clear);
    void waitForXmodemUpdate(int timeout);
    void explicitClientClose();

    void xmodemHandler();

    //getters and setters
    [[nodiscard]] btconState getState() const;
    void setState(btconState stateNew);

private:

    sockaddr_rc localAddress = {0}, clientAddress = {0};
    socklen_t clientAddressLen = sizeof(clientAddress);
    int socketPort, clientPort;

    std::list<uint8_t> buffer;
    std::list<xmodem>  xmodemPackets;

    std::atomic<btconState> state = initial;
    std::mutex listLock;
    std::mutex xmodemLock;

    std::condition_variable bufferUpdate;
    std::condition_variable xmodemUpdate;
};

#endif //PI2_BTCON_H
