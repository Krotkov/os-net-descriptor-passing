//
// Created by daniil on 09.06.19.
//

#include "helper.h"
#include <zconf.h>
#include <sys/socket.h>
#include <vector>

void writeInfo(int fd, const std::string& text) {
    size_t write_size = 0;
    while (write_size != text.length() + 1) {
        ssize_t cur_write_size = ::write(fd, text.c_str() + write_size, text.length() + 1 - write_size);
        if (cur_write_size == -1) {
            perror("Error while writing");
            break;
        }
        write_size += cur_write_size;
    }
}

std::string readInfo(int fd) {
    std::vector<char> buffer(2048);
    size_t read_size = 0;
    while (read_size == 0 || buffer[read_size - 1] != '\0') {
        ssize_t cur_read_size = ::read(fd, buffer.data() + read_size, 2048);
        if (cur_read_size == -1) {
            perror("Error while reading");
            break;
        }
        read_size += cur_read_size;
    }
    buffer.resize(read_size);
    return std::string(buffer.data());
}