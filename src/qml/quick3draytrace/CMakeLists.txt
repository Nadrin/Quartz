set(MODULE_NAME "quick3draytraceplugin")
set(MODULE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

find_package(Qt5 COMPONENTS Core Qml REQUIRED)
find_program(QMLPLUGINDUMP NAMES qmlplugindump qmlplugindump.exe HINTS $ENV{QTDIR}/bin)

add_library(${MODULE_NAME}
    qtquick3draytraceplugin.cpp
    qtquick3draytraceplugin.h
    qmldir
)

target_include_directories(${MODULE_NAME}
    PUBLIC ${QUARTZ_PUBLIC_API}
)

target_compile_features(${MODULE_NAME} PUBLIC cxx_std_14)
target_link_libraries(${MODULE_NAME} Qt5::Core Qt5::Qml Qt3DRaytrace)

target_compile_definitions(${MODULE_NAME}
    PUBLIC QUARTZ_SHARED
    PRIVATE QT3DQUICK3DRAYTRACEPLUGIN_LIBRARY
)

add_custom_command(
    TARGET ${MODULE_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/qml/Qt3D/Raytrace"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/qmldir" "${PROJECT_BINARY_DIR}/qml/Qt3D/Raytrace"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${MODULE_NAME}>" "${PROJECT_BINARY_DIR}/qml/Qt3D/Raytrace"
    COMMAND ${QMLPLUGINDUMP} -nonrelocatable Qt3D.Raytrace 1.0 "${PROJECT_BINARY_DIR}/qml" > "${PROJECT_BINARY_DIR}/qml/Qt3D/Raytrace/plugins.qmltypes"
)
