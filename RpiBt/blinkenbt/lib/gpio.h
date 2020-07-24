//
// Created by beton on 20.06.2020.
//

#ifndef PI2_GPIO_H
#define PI2_GPIO_H

// C/C++ std lib
#include <cstdlib>
#include <cerrno>
#include <cstdio>
#include <string>
#include <iostream>
#include <list>
#include <vector>
#include <array>
#include <bitset>
#include <utility>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

// this lib
#include "libException.h"

// unix
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


class gpio {
public:

    // constructors
    gpio();
    gpio(uint8_t *portsArr, uint8_t portsCount); // auto open ports

    // destructors
    ~gpio();

    // methods
    // port setup
    bool openPort(int a);
    bool closePort(int a);

    // get info/ports
    std::_List_iterator<std::pair<unsigned char, int>> getPort(int a);
    int ledNum();

    // binding
    bool bind(uint8_t port, uint8_t arrPos);
    bool bindNotify(uint8_t port);

    // set port
    // port action recommended (requies binding)
    void setPortFast(uint8_t n, bool value);
    void setPortNotifyFast(bool value);
    // port action fallback (not recommended)
    static bool setPortFD(int fileDescriptor, bool value);

    // animation
    // set 8 led
    void setArray(std::bitset<8> array);
    void setArray(uint8_t array);
    // playing animation
    void playAnimation(std::vector<uint8_t> &frames, int frameTime, int times = 1);
    // animation daemon
    void rollAnimation();
    void loadAnimation(std::vector<uint8_t> ani);
    void triggerAnimation();
    void closeAnimation();
    // set frame time
    void setFrameTime(int ft);

private:

    std::list<std::pair<uint8_t, int>> openPorts; // first - port number
                                                  // second - file descriptor
    std::array<std::list<std::pair<uint8_t, int>>::iterator, 8> binding;
    std::list<std::pair<uint8_t, int>>::iterator                bindingNotify;

    // misc
    int frametime = 100000;

    // threads
    std::thread animationPlayer;
    std::thread animationManager;
    std::thread animationTracker;
    // animation manager
    // parameters
    std::atomic<bool> keepAnimation;
    std::vector<uint8_t> currentAnimation;
    // mutexes, conditional variables
    std::mutex animationLock;
    std::mutex animationTrigger;
    std::condition_variable animationCV;

};


#endif //PI2_GPIO_H
