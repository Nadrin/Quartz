cmake_minimum_required(VERSION 3.8)

set(MODULE_NAME "Qt3DRaytrace")
set(MODULE_LIBNAME "Qt53DRaytrace")
set(MODULE_API ${QUARTZ_PUBLIC_API}/${MODULE_NAME})
set(MODULE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

find_package(Qt5 COMPONENTS Core Gui 3DCore REQUIRED)

set(SOURCES_PRIVATE
    qt3draytrace_global_p.h
    qraytraceaspect.cpp
    qraytraceaspect_p.h
    backend/managers_p.h
    backend/backendnode.cpp
    backend/backendnode_p.h
    backend/entity.cpp
    backend/entity_p.h
)

set(SOURCES_PUBLIC
    ${MODULE_API}/qt3draytrace_global.h
    ${MODULE_API}/qraytraceaspect.h
    ${MODULE_API}/qabstractrenderer.h
)

add_library(${MODULE_NAME} ${SOURCES_PRIVATE} ${SOURCES_PUBLIC})
set_target_properties(${MODULE_NAME} PROPERTIES OUTPUT_NAME ${MODULE_LIBNAME})

target_include_directories(${MODULE_NAME} PUBLIC ${QUARTZ_PUBLIC_API})
target_compile_features(${MODULE_NAME} PUBLIC cxx_std_14)
target_compile_definitions(${MODULE_NAME} PRIVATE VK_NO_PROTOTYPES)
target_link_libraries(${MODULE_NAME} Qt5::Core Qt5::Gui Qt5::3DCorePrivate)

add_subdirectory(renderers)
add_dependencies(${MODULE_NAME} RendererShaders)
target_link_libraries(${MODULE_NAME} ${RENDERER_LIBRARIES})

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${MODULE_NAME}
        PUBLIC QUARTZ_SHARED
        PRIVATE QT3DRAYTRACE_LIBRARY
    )
endif()
