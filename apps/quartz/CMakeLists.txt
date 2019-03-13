cmake_minimum_required(VERSION 3.8)

set(APP_NAME "quartz")

find_package(Qt5 COMPONENTS Core Gui Widgets Qml REQUIRED)

add_executable(${APP_NAME}
    main.cpp
    renderwindow.cpp
    renderwindow.h
    imagewriter.cpp
    imagewriter.h
    version.h
)

if(WIN32)
    target_sources(${APP_NAME} PRIVATE resources/quartz.rc)
endif()

target_compile_features(${APP_NAME} PRIVATE cxx_std_14)
target_include_directories(${APP_NAME} PRIVATE ${QUARTZ_3RDPARTY})
target_link_libraries(${APP_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Qml Qt3DRaytrace Qt3DRaytraceExtras stb)
