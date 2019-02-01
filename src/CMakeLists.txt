cmake_minimum_required(VERSION 3.8)

set(QUARTZ_PUBLIC_API ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(raytrace)
add_subdirectory(extras)
add_subdirectory(qml)
