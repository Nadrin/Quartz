cmake_minimum_required(VERSION 3.8)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(QUARTZ_3RDPARTY ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

add_subdirectory(spirv_reflect)
add_subdirectory(volk)
add_subdirectory(vma)
