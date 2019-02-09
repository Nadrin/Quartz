function(qml_typeinfo QML_PATH QML_VERSION OUTPUT_PATH)
    find_program(QMLPLUGINDUMP NAMES qmlplugindump qmlplugindump.exe HINTS ${QTDIR}/bin)
    if(WIN32)
        add_custom_command(
            TARGET ${MODULE_NAME}
            POST_BUILD
            COMMAND
                ${CMAKE_COMMAND} -E env "PATH=${QTDIR}/bin;${THIRDPARTY_PATH};${MODULE_PATH};$ENV{PATH}"
                ${QMLPLUGINDUMP} -nonrelocatable ${QML_PATH} ${QML_VERSION} "${PROJECT_BINARY_DIR}/qml" > "${PROJECT_BINARY_DIR}/qml/${OUTPUT_PATH}")
    else()
        add_custom_command(
            TARGET ${MODULE_NAME}
            POST_BUILD
            COMMAND
                ${QMLPLUGINDUMP} -nonrelocatable ${QML_PATH} ${QML_VERSION} "${PROJECT_BINARY_DIR}/qml" > "${PROJECT_BINARY_DIR}/qml/${OUTPUT_PATH}")
    endif()
endfunction()

if(BUILD_SHARED_LIBS)
    add_subdirectory(quick3draytrace)
    add_subdirectory(quick3draytraceextras)
endif()
