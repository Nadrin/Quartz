cmake_minimum_required(VERSION 3.8)

set(MODULE_NAME "Qt3DRaytraceExtras")
set(MODULE_LIBNAME "Qt53DRaytraceExtras")
set(MODULE_API ${QUARTZ_PUBLIC_API}/${MODULE_NAME})

find_package(Qt5 COMPONENTS Core Gui 3DCore 3DInput 3DLogic REQUIRED)

set(SOURCES_PRIVATE
    qt3draytraceextras_global_p.h
    qt3dwindow.cpp
    qt3dwindow_p.h
)

set(SOURCES_PUBLIC
    ${MODULE_API}/qt3draytraceextras_global.h
    ${MODULE_API}/qt3dwindow.h
)

add_library(${MODULE_NAME} ${SOURCES_PRIVATE} ${SOURCES_PUBLIC})
set_target_properties(${MODULE_NAME} PROPERTIES OUTPUT_NAME ${MODULE_LIBNAME})
set_target_properties(${MODULE_NAME} PROPERTIES OUTPUT_NAME_DEBUG "${MODULE_LIBNAME}d")

target_include_directories(${MODULE_NAME} PUBLIC ${QUARTZ_PUBLIC_API})
target_compile_features(${MODULE_NAME} PUBLIC cxx_std_14)
target_link_libraries(${MODULE_NAME} Qt5::Core Qt5::GuiPrivate Qt5::3DCore Qt5::3DInput Qt5::3DLogic Qt3DRaytrace)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${MODULE_NAME}
        PUBLIC QUARTZ_SHARED
        PRIVATE QT3DRAYTRACEEXTRAS_LIBRARY
    )
endif()
