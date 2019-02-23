cmake_minimum_required(VERSION 3.8)
project(libstb)

add_library(stb STATIC
    libstb.c
    stb_image.h
    stb_image_write.h
)
target_include_directories(stb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
