cmake_minimum_required(VERSION 3.8)

find_program(GLSLANG NAMES glslangValidator glslangValidator.exe HINTS /opt/glslang/bin)

file(GLOB_RECURSE GLSL_SOURCES "*.glsl")
foreach(GLSL_PATH ${GLSL_SOURCES})
    get_filename_component(GLSL_FILE ${GLSL_PATH} NAME)
    string(REGEX REPLACE "\\.[^.]*$" "" GLSL_BASE_FILE ${GLSL_FILE})
    set(SPV_PATH "${PROJECT_BINARY_DIR}/shaders/${GLSL_BASE_FILE}.spv")
    add_custom_command(
        OUTPUT ${SPV_PATH}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
        COMMAND ${GLSLANG} -V ${GLSL_PATH} -o ${SPV_PATH}
        DEPENDS ${GLSL_PATH})
    list(APPEND SPV_BINARY_FILES ${SPV_PATH})
endforeach(GLSL_PATH)

add_custom_target(VulkanShaders DEPENDS ${SPV_BINARY_FILES})
