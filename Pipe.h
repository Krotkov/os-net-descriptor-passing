//
// Created by daniil on 09.06.19.
//

#ifndef OS_NET_DESCRIPTOR_PASSING_PIPE_H
#define OS_NET_DESCRIPTOR_PASSING_PIPE_H


#include <bits/stl_pair.h>

class Pipe {
    int fds[2];

public:
    Pipe();

    void create();

    std::pair<int, int> getFds();

    void close();
};


#endif //OS_NET_DESCRIPTOR_PASSING_PIPE_H
