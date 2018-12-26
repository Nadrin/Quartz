cmake_minimum_required(VERSION 3.8)
project(vma)

find_package(Vulkan REQUIRED)

add_library(vma STATIC vk_mem_alloc.cpp vk_mem_alloc.h)
target_include_directories(vma PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(vma Vulkan::Vulkan)
