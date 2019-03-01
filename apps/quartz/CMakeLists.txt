cmake_minimum_required(VERSION 3.8)

set(APP_NAME "quartz")

find_package(Qt5 COMPONENTS Core Gui Widgets 3DCore REQUIRED)

add_executable(${APP_NAME}
    main.cpp
    renderwindow.cpp
    renderwindow.h
)

target_compile_features(${APP_NAME} PRIVATE cxx_std_14)
target_include_directories(${APP_NAME} PRIVATE ${QUARTZ_3RDPARTY})
target_link_libraries(${APP_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::3DCore Qt3DRaytrace Qt3DRaytraceExtras stb)
