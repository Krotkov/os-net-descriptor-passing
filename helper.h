//
// Created by daniil on 09.06.19.
//

#ifndef OS_NET_DESCRIPTOR_PASSING_HELPER_H
#define OS_NET_DESCRIPTOR_PASSING_HELPER_H

#include <string>

void writeInfo(int fd, const std::string& text);

std::string readInfo(int fd);

#endif //OS_NET_DESCRIPTOR_PASSING_HELPER_H
