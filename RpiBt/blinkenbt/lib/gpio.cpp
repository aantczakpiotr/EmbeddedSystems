//
// Created by beton on 20.06.2020.
//

#include "gpio.h"

gpio::gpio() : keepAnimation(false) {

}

gpio::gpio(u_int8_t *portsArr, u_int8_t portsCount) : keepAnimation(false) {
    for (uint8_t i = 0; i < portsCount; i++)
        openPort(portsArr[i]);
}

gpio::~gpio() {
    while (!openPorts.empty()) {
        closePort(openPorts.back().first);
    }
}

bool gpio::openPort(int a) {

    int fileDescriptor;
    std::string str;
    std::string path;

    try {
        // use gpio port capabilities, export port for handling
        path = "/sys/class/gpio/export";
        if ((fileDescriptor = open(path.c_str(), O_WRONLY)) == -1)
            throw fileDescriptorOpenException();

        str = std::to_string(a);

        if (write(fileDescriptor, str.c_str(), str.size()) != str.size())
            throw fileDescriptorWriteException();

        close(fileDescriptor);

        // set direction to "out"
        path = "/sys/class/gpio/gpio" + str + "/direction";
        if ((fileDescriptor = open(path.c_str(), O_WRONLY)) == -1)
            throw fileDescriptorOpenException();

        if (write(fileDescriptor, "out", 3) != 3)
            throw fileDescriptorWriteException();

        // port was set successfully -> export file descriptor
        path = "/sys/class/gpio/gpio" + str + "/value";
        if ((fileDescriptor = open(path.c_str(), O_WRONLY)) == -1)
            throw fileDescriptorOpenException();

    } catch (fileDescriptorOpenException &e) {
        return false;
    } catch (fileDescriptorWriteException &e) {
        close(fileDescriptor);
        return false;
    }

    std::pair<uint8_t, int> port;

    port.first = a;
    port.second = fileDescriptor;

    openPorts.push_back(port);

    return true;
}

bool gpio::closePort(int a) {
    int fd;
    auto port = getPort(a);
    if ((fd = port->second) == -1) {
        return false;
    }
    else {
        close(fd);

        int fileDescriptor;
        std::string str, path;

        try {
            // close gpio port capabilities, unexport port
            path = "/sys/class/gpio/unexport";
            if ((fileDescriptor = open(path.c_str(), O_WRONLY)) == -1)
                throw fileDescriptorOpenException();

            str = std::to_string(a);

            if (write(fileDescriptor, str.c_str(), str.size()) != str.size())
                throw fileDescriptorWriteException();

            close(fileDescriptor);

        } catch (fileDescriptorOpenException &e) {
            return false;
        } catch (fileDescriptorWriteException &e) {
            close(fileDescriptor);
            return false;
        }

        openPorts.remove(*port);

        return true;
    }
}

bool gpio::setPortFD(int fileDescriptor, bool value) {
    return write(fileDescriptor, (value ? "1" : "0"), 1) == 1;
}

std::_List_iterator<std::pair<unsigned char, int>> gpio::getPort(int a) {
    if (openPorts.empty()) return std::_List_iterator<std::pair<unsigned char, int>>();

    auto iter = openPorts.begin();
    for (int i = 0; i < openPorts.size(); i++, iter++) {
        if (iter->first == a)
            return iter;
    }
    return std::_List_iterator<std::pair<unsigned char, int>>();
}

int gpio::ledNum() {
    return openPorts.size();
}

bool gpio::bind(uint8_t port, uint8_t arrPos) {
    auto get = getPort(port);
    if (get->first > 0) {
        binding[arrPos] = get;
    }
    return false;
}

bool gpio::bindNotify(uint8_t port) {
    auto get = getPort(port);
    if (get->first > 0) {
        bindingNotify = get;
    }
    return false;
}

void gpio::setPortFast(uint8_t n, bool value) {
    write(binding[n]->second, (value ? "1" : "0"), 1);
}

void gpio::setPortNotifyFast(bool value) {
    write(bindingNotify->second, (value ? "1" : "0"), 1);
}

void gpio::setArray(std::bitset<8> array) {
    for (uint8_t i = 0; i < 8; i++)
        setPortFast(i, array.test(i));
}

void gpio::setArray(uint8_t array) {
    for (uint8_t i = 0; i < 8; i++)
        setPortFast(i, array & 0x01u << i);
}

void gpio::playAnimation(std::vector<uint8_t> &array, int frameTime, int times) {
    while (times-- > 0) {
        for (uint8_t & i : array) {
            animationPlayer = std::thread(
                [this](uint8_t v)
                {
                    setArray(v);
                }, i);
            usleep(frameTime);
            animationPlayer.join();
        }
    }
}

void gpio::loadAnimation(std::vector<uint8_t> ani) {
    std::lock_guard vectorLock(animationLock);
    // in case something breaks, don't use std::move
    currentAnimation = /*std::move*/(ani);
}

void gpio::rollAnimation() {
    keepAnimation = true;
    while (keepAnimation) {
        std::unique_lock lock(animationTrigger);
        animationCV.wait(lock);
        std::cout << "freedom\n";

        if (!keepAnimation) break;

        {
            std::lock_guard vectorLock(animationLock);
            animationTracker = std::thread(
                [this](std::vector<uint8_t> f, int ft)
                {
                    playAnimation(f, ft);
                },
                currentAnimation, frametime);
        }
        animationTracker.join();
        std::cout << "roll join\n";
    }
}

void gpio::triggerAnimation() {
    animationCV.notify_one();
}

void gpio::closeAnimation() {
    keepAnimation = false;
    triggerAnimation();
}

void gpio::setFrameTime(int ft) {
    frametime = ft;
}


