cmake_minimum_required(VERSION 3.10)
project(hw6)

set(CMAKE_CXX_STANDARD 14)

set(UTILS stream_socket.cpp stream_socket.h utils.h utils.cpp)

add_executable(server server.cpp ${UTILS})

add_executable(client client.cpp ${UTILS})