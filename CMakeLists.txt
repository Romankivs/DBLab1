cmake_minimum_required(VERSION 3.20)
project(DBLab1 C)

set(CMAKE_C_STANDARD 11)

add_executable(DBLab1 main.c tables.h master.h slave.h error.h print.h garbage.h input.h slave.c master.c garbage.c error.c)
