cmake_minimum_required(VERSION 3.9.4)

project(modulo_lsystem LANGUAGES CXX)

set(exe_src main.cpp modulo_int_system.cpp)

add_executable(${PROJECT_NAME} ${exe_src})
