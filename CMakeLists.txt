cmake_minimum_required(VERSION 3.17)
project(gpgpu_exercise02 C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_BUILD_TYPE Debug)

add_executable(main_semaphores main.c)
target_link_libraries(main_semaphores pthread)

add_executable(main_cond_vars main_cond_vars.c)
target_link_libraries(main_cond_vars pthread)