cmake_minimum_required(VERSION 3.26)
project(OS C)

set(CMAKE_C_STANDARD 23)

add_executable(OS
        HW5/receiver.c
        HW5/sender.c)
