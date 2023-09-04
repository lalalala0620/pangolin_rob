cmake_minimum_required(VERSION 3.10)

project(SocketExample)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加可执行文件
add_executable(Servo servo_test.cc)
# add_executable(Control Pangolin_controller.cpp)
# add_executable(Client socket_client.cc)
# add_executable(Server_learning socket_learning.cc)
# add_executable(Server_joystick socket_joystick.cc)
# # add_executable(Server_joystick socket_joystick.cc)
# # add_executable(tcp_server tcp-server.cc)
# add_executable(socket_test socket_test.cc)

include_directories(./include)

# # target_link_libraries(tcp_server pthread)
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} <pangolin_c/test/Findpigpio.cmake> )

# cmake_minimum_required(VERSION 3.0)

# project(SampleEventProvider)

# # 设置C++标准
# set(CMAKE_CXX_STANDARD 11)

# # 寻找pthread库
# find_package(Threads REQUIRED)

# # 添加pigpio库的链接目录
# link_directories(${CMAKE_SOURCE_DIR})

# # 添加可执行文件
# add_executable(drive servo_test.cc)

# # 链接pthread库
# target_link_libraries(drive Threads::Threads)

# # 链接pigpio库
# target_link_libraries(drive pigpio)