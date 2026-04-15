cmake_minimum_required(VERSION 2.8)

project(TcpServer)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)

# 头文件路径
include_directories(include)

# 生成可执行程序
add_executable(tcpserver_cmake main.cpp src/server.cpp)

# 链接线程库
target_link_libraries(tcpserver_cmake pthread)