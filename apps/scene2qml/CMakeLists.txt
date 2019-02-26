cmake_minimum_required(VERSION 3.8)

set(APP_NAME "scene2qml")

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(assimp REQUIRED)

add_executable(${APP_NAME}
    main.cpp
    importer.cpp
    importer.h
    exporter.cpp
    exporter.h
    scene.h
)

target_include_directories(${APP_NAME}
    PRIVATE ${QUARTZ_3RDPARTY}
    PRIVATE ${assimp_INCLUDE_DIRS}
)

target_compile_features(${APP_NAME} PRIVATE cxx_std_14)
target_link_libraries(${APP_NAME} Qt5::Core ${assimp_LIBRARIES})
