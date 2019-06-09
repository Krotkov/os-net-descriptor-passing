//
// Created by daniil on 27.05.19.
//

#include <fcntl.h>
#include <vector>
#include <zconf.h>
#include <iostream>
#include "Socket.h"
#include <unistd.h>
#include <sys/un.h>

Socket::Socket() : fd(socket(AF_UNIX, SOCK_STREAM, 0)) {
    if (fd == -1) {
        throw socket_exception("Can't create socket");
    }
    flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        throw socket_exception("Can't get socket status");
    }
}

Socket &Socket::operator=(Socket &&other) noexcept {
    fd = other.fd;
    other.fd = -1;
    return *this;
}

Socket::Socket(Socket &&other) noexcept : fd(other.fd) {
    other.fd = -1;
}

void Socket::bind(const char *file_name) {
    sockaddr_un address{};
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, file_name);
    if (::bind(fd, reinterpret_cast<sockaddr *>(&address),
               sizeof(address)) == -1) {
        throw socket_exception("Can't bind socket");
    }
}

void Socket::listen(int connections) {
    if (::listen(fd, connections) == -1) {
        throw socket_exception("Can't set socket to listening state");
    }
}

Socket Socket::accept() {
    int new_descriptor = ::accept(fd, nullptr, nullptr);
    if (fd == -1) {
        throw socket_exception("Can't accept new connection");
    }
    return Socket(new_descriptor);
}

Socket::Socket(int descriptor) : fd(descriptor) {}

int Socket::connect(const char *file_name) {
    sockaddr_un address{};
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, file_name);
    if (::connect(fd, reinterpret_cast<sockaddr *>(&address),
                  sizeof(address)) == -1) {
        throw socket_exception("Can't connect to server");
    }
    return 0;
}

int Socket::getDescriptor() {
    return fd;
}

size_t Socket::send(std::string &data) {
    size_t write_size = 0;
    while (write_size != data.size() + 1) {
        ssize_t cur_write_size = ::send(fd, data.c_str() + write_size, data.size() + 1 - write_size, 0);
        if (cur_write_size == -1) {
            throw socket_exception("Error while sending");
        }
        write_size += cur_write_size;
    }
    return static_cast<size_t>(write_size);
}

std::string Socket::read() {
    std::vector<char> buffer(BUFFER_SIZE);
    size_t read_size = 0;
    while (read_size == 0 || buffer[read_size - 1] != '\0') {
        ssize_t cur_read_size = ::recv(fd, buffer.data() + read_size, BUFFER_SIZE, 0);
        if (cur_read_size == -1) {
            throw socket_exception("Error while reading");
        }
        read_size += cur_read_size;
    }
    buffer.resize(read_size);
    return std::string(buffer.data());
}

void Socket::unblock() {
    if (!(flags & O_NONBLOCK)) {
        if (::fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
            throw socket_exception("Can't change socket status");
        }
        flags |= O_NONBLOCK;
    }
}

void Socket::close() {
    if (fd != -1) {
        if (::close(fd) == -1) {
            std::cerr << "Can't close socket" << std::endl;
        }
    }
}

void Socket::sendFileDescriptor(int fileDescriptor) {
    struct msghdr msg = {nullptr};
    char buf[CMSG_SPACE(sizeof(fileDescriptor))];
    memset(buf, 0, sizeof(buf));

    struct iovec io = {.iov_base = (void *) "", .iov_len = 1};

    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(fileDescriptor));

    memmove(CMSG_DATA(cmsg), &fileDescriptor, sizeof(fileDescriptor));

    msg.msg_controllen = cmsg->cmsg_len;

    if (sendmsg(fd, &msg, 0) < 0) {
        throw socket_exception("Error sending file descriptor: ");
    }
}

int Socket::getFileDescriptor() {
    msghdr message = {0};
    cmsghdr *structcmsghdr;
    char buffer[CMSG_SPACE(sizeof(int))];
    char duplicate[512];
    bzero(buffer, sizeof(buffer));
    iovec io = {.iov_base = &duplicate, .iov_len = sizeof(duplicate)};

    message.msg_control = buffer;
    message.msg_controllen = sizeof(buffer);
    message.msg_iov = &io;
    message.msg_iovlen = 1;

    if (recvmsg(fd, &message, 0) < 0) {
        return -1;
    }

    structcmsghdr = CMSG_FIRSTHDR(&message);
    int received_fd;
    memcpy(&received_fd, (int *) CMSG_DATA(structcmsghdr), sizeof(int));
    return received_fd;
}



