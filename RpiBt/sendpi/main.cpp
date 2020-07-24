#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <array>
#include "xmodem.h"

void loadSeq(std::string fileName, std::vector<uint8_t> &tab) {
    std::ifstream file(fileName);

    tab.clear();

    while (!file.eof()) {
        int num;
        file >> num;
        tab.push_back((uint8_t)num);
    }
    file.close();
}

struct xMsg {
    std::vector<xmodem> packet;
};

void convertSeqToX(xMsg &msg, std::vector<uint8_t> &seq) {
    msg.packet.clear();
    std::array<uint8_t, 128> data{};
    uint8_t counter = 0;
    std::list<uint8_t> seqlist;
    uint8_t packetNumber = 1;

    seqlist.resize(seq.size(), 0);
    std::copy(seq.begin(), seq.end(), seqlist.begin());

    while (!seqlist.empty()) {
        data[counter++] = seqlist.front();
        seqlist.pop_front();
        if (counter == 128) {
            counter = 0;
            xmData x{};
            std::copy(data.begin(), data.end(), std::begin(x.byte));
            data.fill(0);
            msg.packet.emplace_back(x, packetNumber++);
        }
    }

    if (counter > 0) {
        xmData x{};
        std::copy(data.begin(), data.end(), std::begin(x.byte));
        msg.packet.emplace_back(x, packetNumber);
    }

    xmPacket ending{};
    ending.header = xmodem::xmACK;
    msg.packet.emplace_back(ending);
}


//client
int main(int argc, char **argv)
{
    int s;

    struct sockaddr_rc addr = { 0 };
    char dest[18] = "B8:27:EB:46:9D:46";

    // allocate a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // set the connection parameters (who to connect to)
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba( dest, &addr.rc_bdaddr );

    std::cout << "connecting\n";
    // connect to server
    connect(s, (struct sockaddr *) &addr, sizeof(addr));
    // send a message

    std::cout << "connected\n";

    std::vector<uint8_t> sq1, sq2, sq3, sq4;
    xMsg                 xq1, xq2, xq3, xq4;

    loadSeq("moveLeft.txt", sq1);
    loadSeq("moveRight.txt", sq2);
    loadSeq("moveBoom.txt", sq3);
    loadSeq("moveEnd.txt", sq4);

    convertSeqToX(xq1, sq1);
    convertSeqToX(xq2, sq2);
    convertSeqToX(xq3, sq3);
    convertSeqToX(xq4, sq4);

    std::cout << "4 sequences: \n"
              << "xq1: " << xq1.packet.size() << " packets\n"
              << "xq2: " << xq2.packet.size() << " packets\n"
              << "xq3: " << xq3.packet.size() << " packets\n"
              << "xq4: " << xq4.packet.size() << " packets\n";

    std::cout << "sending sequence #1\n\n";
    for (auto & i : xq1.packet) {
        uint8_t answer;
        do {
            if (i.getPacket()[0] != xmodem::xmACK) {
                std::cout << "sending packet #" << (int)i.getPacket()[1] << "\n";
            } else {
                std::cout << "sending packet ACK" << "\n";
            }
            std::array<uint8_t, 132> a = i.getPacket();
            write(s, a.data(), sizeof(a));
            std::cout << "wait for ACK\n";
            read(s, &answer, 1);
        } while (answer != xmodem::xmACK);
    }
    std::cout << "done... waiting 5 seconds\n\n";
    sleep(5);
    std::cout << "sending sequence #2\n\n";
    for (auto & i : xq2.packet) {
        uint8_t answer;
        do {
            if (i.getPacket()[0] != xmodem::xmACK) {
                std::cout << "sending packet #" << (int)i.getPacket()[1] << "\n";
            } else {
                std::cout << "sending packet ACK" << "\n";
            }
            std::array<uint8_t, 132> a = i.getPacket();
            write(s, a.data(), sizeof(a));
            std::cout << "wait for ACK\n";
            read(s, &answer, 1);
        } while (answer != xmodem::xmACK);
    }
    std::cout << "done... waiting 5 seconds\n\n";
    sleep(5);
    std::cout << "sending sequence #3\n\n";
    for (auto & i : xq3.packet) {
        uint8_t answer;
        do {
            if (i.getPacket()[0] != xmodem::xmACK) {
                std::cout << "sending packet #" << (int)i.getPacket()[1] << "\n";
            } else {
                std::cout << "sending packet ACK" << "\n";
            }
            std::array<uint8_t, 132> a = i.getPacket();
            write(s, a.data(), sizeof(a));
            std::cout << "wait for ACK\n";
            read(s, &answer, 1);
        } while (answer != xmodem::xmACK);
    }
    std::cout << "done... not waiting\n\n";
    std::cout << "sending sequence #4\n\n";
    for (auto & i : xq4.packet) {
        uint8_t answer;
        do {
            if (i.getPacket()[0] != xmodem::xmACK) {
                std::cout << "sending packet #" << (int)i.getPacket()[1] << "\n";
            } else {
                std::cout << "sending packet ACK" << "\n";
            }
            std::array<uint8_t, 132> a = i.getPacket();
            write(s, a.data(), sizeof(a));
            std::cout << "wait for ACK\n";
            read(s, &answer, 1);
        } while (answer != xmodem::xmACK);
    }
    std::cout << "done... waiting 2 seconds to cut while the animation is playing\n\n";
    sleep(2);
    std::cout << "ending trasmission\n\n";
    uint8_t nullmsg = xmodem::xmEOT;
    write(s, &nullmsg, 1);
    std::cout << "closing socket\n\n";
    close(s);

    return 0;
}
