cmake_minimum_required(VERSION 3.7)
project(DiskManager)
set(CMAKE_CXX_STANDARD 17)
add_executable(DiskManager src/Source.cpp src/FAT.cpp src/FAT.h)
