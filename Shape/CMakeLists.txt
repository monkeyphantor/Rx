cmake_minimum_required(VERSION 3.16.3)

project(RxProject)

add_library(rx_shape STATIC
Shape.hpp
Shape.cpp
)

target_include_directories(rx_shape PUBLIC ../Core )
target_link_libraries(rx_shape PUBLIC rx_core)
