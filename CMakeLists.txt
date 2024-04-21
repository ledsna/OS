cmake_minimum_required(VERSION 3.26)
project(OS C)

set(CMAKE_C_STANDARD 23)

add_executable(OS
        HW5/receiver.c
        HW5/sender.c
        IHW1/4/4.c
        IHW1/5/5.c
        IHW1/6/6.c
        IHW1/7/7.c
        IHW1/8/sender.c
        IHW1/8/receiver.c
        IHW1/9/receiver.c
        IHW1/9/sender.c
        IHW1/10/sender.c
        IHW1/10/receiver.c
        IHW1/converter.h
        IHW2/4-5/unnamed.c
        IHW2/hive.c
        IHW2/hive.h
        IHW2/hive.h
        IHW2/hive.h
        IHW2/hive.c
        IHW2/hive.c
        IHW2/hive.h
        IHW2/6-7/named.c
)
