cmake_minimum_required(VERSION 3.10)

project(SocketExample)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加可执行文件
add_executable(Server socket_server.cc)
add_executable(Client socket_client.cc)
add_executable(Server_learning socket_learning.cc)
add_executable(Server_joystick socket_joystick.cc)