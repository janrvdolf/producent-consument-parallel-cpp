cmake_minimum_required(VERSION 3.17)
project(gpgpu_exercise02 C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_BUILD_TYPE Debug)

add_executable(gpgpu_exercise02 main.c)
target_link_libraries(gpgpu_exercise02 pthread)