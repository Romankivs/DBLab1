cmake_minimum_required(VERSION 3.20)
project(DBLab1 C)

set(CMAKE_C_STANDARD 11)

add_executable(DBLab1 src/main.c
        include/tables.h
        include/master.h
        include/slave.h
        include/error.h
        include/print.h
        include/garbage.h
        include/input.h
        src/slave.c
        src/master.c
        src/garbage.c
        src/error.c)

target_include_directories(DBLab1 PRIVATE include)