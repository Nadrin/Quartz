cmake_minimum_required(VERSION 3.8)
project(spirv_reflect)

find_package(Vulkan REQUIRED)

add_library(spirv_reflect STATIC spirv_reflect.c spirv_reflect.h)
target_include_directories(spirv_reflect PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(spirv_reflect Vulkan::Vulkan)
