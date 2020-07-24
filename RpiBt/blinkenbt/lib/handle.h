//
// Created by beton on 30.06.2020.
//

#ifndef BLINKENBT_HANDLE_H
#define BLINKENBT_HANDLE_H

#include "gpio.h"
#include "btcon.h"
#include "xmodem.h"

class handle {
public:
    handle();
    ~handle();

    void listen();

    std::list<uint8_t> getAnimation();
    void animate(std::list<uint8_t>& l);
    bool notExited();

private:
    constexpr const static uint8_t leds[9] = {2, 4, 27, 10, 11, 5, 13, 20, 21};

    gpio gpioHandler;
    btcon btHandler;

    //threads
    std::thread aniRoll;
    std::thread infoled;
    std::thread btReader;
    std::thread xmodemReader;
};


#endif //BLINKENBT_HANDLE_H
