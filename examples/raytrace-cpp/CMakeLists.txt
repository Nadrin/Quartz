cmake_minimum_required(VERSION 3.8)

set(APP_NAME "raytrace-cpp")

find_package(Qt5 COMPONENTS Core Gui 3DCore REQUIRED)

add_executable(${APP_NAME} main.cpp)

target_compile_features(${APP_NAME} PRIVATE cxx_std_14)
target_link_libraries(${APP_NAME} Qt5::Core Qt5::Gui Qt5::3DCore Qt3DRaytrace Qt3DRaytraceExtras)
