set(MODULE_NAME "quick3draytraceextrasplugin")
set(MODULE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

find_package(Qt5 COMPONENTS Core Qml REQUIRED)

add_library(${MODULE_NAME}
    qtquick3draytraceextrasplugin.cpp
    qtquick3draytraceextrasplugin.h
    qmldir
)

target_include_directories(${MODULE_NAME}
    PUBLIC ${QUARTZ_PUBLIC_API}
)

target_compile_features(${MODULE_NAME} PUBLIC cxx_std_14)
target_link_libraries(${MODULE_NAME} Qt5::Core Qt5::Qml Qt3DRaytraceExtras)

target_compile_definitions(${MODULE_NAME}
    PUBLIC QUARTZ_SHARED
    PRIVATE QT3DQUICK3DRAYTRACEEXTRASPLUGIN_LIBRARY
)

add_custom_command(
    TARGET ${MODULE_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/qml/Qt3D/RaytraceExtras"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/qmldir" "${PROJECT_BINARY_DIR}/qml/Qt3D/RaytraceExtras"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${MODULE_NAME}>" "${PROJECT_BINARY_DIR}/qml/Qt3D/RaytraceExtras"
)

if(DUMP_QML_TYPEINFO)
    qml_typeinfo("Qt3D.RaytraceExtras" "1.0" "Qt3D/RaytraceExtras/plugins.qmltypes")
endif()
