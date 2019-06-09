//
// Created by daniil on 09.06.19.
//
#include <iostream>
#include <vector>
#include <zconf.h>
#include "Socket.h"
#include "helper.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Wrong numbers of arguments" << std::endl;
    }

    Socket socket;

    socket.connect(argv[1]);

    int sendFd = socket.getFileDescriptor();
    int getFd = socket.getFileDescriptor();

    while (true) {
        std::string text;
        std::cin >> text;
        if (text == "exit") break;
        writeInfo(sendFd, text);
        std::string response = readInfo(getFd);
        std::cout << "Echo: " << response << std::endl;
    }
}
