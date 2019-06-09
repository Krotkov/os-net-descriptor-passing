//
// Created by daniil on 09.06.19.
//
#include <iostream>
#include <vector>
#include <unistd.h>
#include <csignal>
#include <sys/stat.h>
#include <fcntl.h>
#include "Socket.h"
#include "Pipe.h"
#include "helper.h"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Running:" << std::endl <<
             "server <name of file for UNIX socket>" << std::endl;
        return 0;
    }

    struct stat stat_s{};
    if (stat(argv[1], &stat_s) != -1) {
        if (unlink(argv[1]) == -1) {
            perror("Cannot unlink file for UNIX socket");
            return 0;
        }
    }

    Socket server_socket;
    server_socket.bind(argv[1]);
    server_socket.listen();
    Socket client_socket = server_socket.accept();

    Pipe pipeToSend;
    Pipe PipeToWrite;
    pipeToSend.create();
    PipeToWrite.create();

    client_socket.sendFileDescriptor(pipeToSend.getFds().second);
    client_socket.sendFileDescriptor(PipeToWrite.getFds().first);
    std::cout << "New client connected" << std::endl;
    while (true) {
        if (client_socket.getDescriptor() == -1) break;

        int writeFd = PipeToWrite.getFds().second;
        int readFd = pipeToSend.getFds().first;

        std::string str = readInfo(readFd);

        writeInfo(writeFd, str);
    }
    return 0;
}
