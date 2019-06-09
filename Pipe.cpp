//
// Created by daniil on 09.06.19.
//

#include <zconf.h>
#include <stdexcept>
#include <cstring>
#include "Pipe.h"

Pipe::Pipe() {
    fds[0] = -1;
    fds[1] = -1;
}

void Pipe::create() {
    if (pipe(fds) == -1) {
        throw std::runtime_error("Error creating pipe");
    }
}

std::pair<int, int> Pipe::getFds() {
    return {fds[0], fds[1]};
}

void Pipe::close() {
    for (int fd : fds) {
        if (::close(fd) == -1) {
            throw std::runtime_error("Error closing pipe");
        }
    }
}



