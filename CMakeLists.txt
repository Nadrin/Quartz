cmake_minimum_required(VERSION 3.8)
project(Quartz)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_APPS "Build the standalone renderer & supplemental tools" ON)
option(BUILD_EXAMPLES "Build example programs" ON)

option(DUMP_QML_TYPEINFO "Dump QML type information for use in QtCreator" OFF)

set(QTDIR "$ENV{QTDIR}" CACHE STRING "Path to Qt SDK")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${QTDIR}/lib/cmake")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DQUARTZ_DEBUG")

add_subdirectory(3rdparty)
add_subdirectory(src)

if(BUILD_APPS)
    add_subdirectory(apps)
endif()
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

set(QML_IMPORT_PATH "${PROJECT_BINARY_DIR}/qml")
