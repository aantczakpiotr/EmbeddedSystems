//
// Created by beton on 30.06.2020.
//

#include "handle.h"

handle::handle() : gpioHandler(const_cast<uint8_t *>(leds), 9), btHandler() {
    gpioHandler.bindNotify(2);
    for (int i = 0; i < 8; i++)
        gpioHandler.bind(leds[i + 1], i);
    gpioHandler.setFrameTime(20000);

    aniRoll = std::thread(
            [this]() {
                gpioHandler.rollAnimation();
            });

    infoled = std::thread(
            [](gpio* g, btcon* b) {
                g->setPortNotifyFast(true);
                bool var = true;
                while (var) {
                    usleep(20000);
                    btconState st = b->getState();

                    switch (st) {
                        case initial:
                            g->setPortNotifyFast(true);
                            break;
                        case listeningWaiting:
                            g->setPortNotifyFast(true);
                            usleep(20000);
                            g->setPortNotifyFast(false);
                            usleep(300000);
                            break;
                        case listenedAccepted:
                            for (int i = 0; i < 10; i++) {
                                g->setPortNotifyFast(true);
                                usleep(100000);
                                g->setPortNotifyFast(false);
                                usleep(100000);
                            }
                            break;
                        case listenedDenied:
                            for (int i = 0; i < 4; i++) {
                                g->setPortNotifyFast(true);
                                usleep(50000);
                                g->setPortNotifyFast(false);
                                usleep(50000);
                                g->setPortNotifyFast(true);
                                usleep(50000);
                                g->setPortNotifyFast(false);
                                usleep(300000);
                            }
                            var = false;
                            break;
                        case connected:
                            g->setPortNotifyFast(false);
                            usleep(100000);
                            break;
                        case exited:
                            var = false;
                            break;
                    }

                }

            }, &gpioHandler, &btHandler);

    xmodemReader = std::thread([this](){btHandler.xmodemHandler();});
}

handle::~handle() {
    btHandler.setState(exited);
    btHandler.explicitClientClose();
    gpioHandler.closeAnimation();
    gpioHandler.triggerAnimation();
    std::cout << "joining aniRoll\n";
    aniRoll.join();
    std::cout << "joining infoled\n";
    infoled.join();
    std::cout << "joining btReader\n";
    btReader.join();
    std::cout << "joining xmodemReader\n";
    xmodemReader.join();
    std::cout << "all threads joined\n";
}

void handle::listen() {
    btHandler.setState(listeningWaiting);
    btHandler.listen();
    btHandler.setState(listenedAccepted);
    btReader = std::thread(
            [this]() {
                btHandler.handleConnection();
            });
}

std::list<uint8_t> handle::getAnimation() {
    std::list<uint8_t> animation;
    bool done;
    do {
        done = btHandler.extractXmodemMessage(animation, false);
        btHandler.waitForXmodemUpdate(10);
        if (btHandler.getState() == exited) {
            animation.clear();
            animation.push_back(0);
            return animation;
        }
    } while (!done);
    //clear ending
    while (!animation.empty() && animation.back() == 0) {
        animation.pop_back();
    }
    animation.push_back(0);

    std::cout << "\nreformated animation: " << animation.size() << "\n\n";
    return animation;
}

void handle::animate(std::list<uint8_t> &l) {
    if (btHandler.getState() == exited) {
        return;
    }
    std::vector<uint8_t> v(l.begin(), l.end());
    gpioHandler.loadAnimation(v);
    gpioHandler.triggerAnimation();
}

bool handle::notExited() {
    return btHandler.getState() != exited;
}

