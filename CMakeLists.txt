cmake_minimum_required(VERSION 3.9)

project(os-net-descriptor-passing)

add_executable(client mainClient.cpp Socket.cpp Pipe.cpp helper.cpp)

add_executable(server mainServer.cpp Socket.cpp Pipe.cpp helper.cpp)