target_sources(${MODULE_NAME} PRIVATE renderers/vulkan/shaders/vulkan_shaders.qrc)

set(GLSL_SOURCES
    display.frag.glsl
    display.vert.glsl
    pathtrace.rgen.glsl
    pathtrace.rmiss.glsl
    pathtrace.rchit.glsl
    queryemission.rchit.glsl
    queryemission.rmiss.glsl
    queryvisibility.rchit.glsl
    queryvisibility.rmiss.glsl
)

find_program(GLSLANG NAMES glslangValidator glslangValidator.exe
    HINTS /opt/glslang/bin "$ENV{VULKAN_SDK}/bin")

add_custom_target(VulkanShaders)
foreach(GLSL_SOURCE_ITEM ${GLSL_SOURCES})
    set(GLSL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${GLSL_SOURCE_ITEM}")
    get_filename_component(GLSL_FILE ${GLSL_PATH} NAME)
    string(REGEX REPLACE "\\.[^.]*$" "" GLSL_BASE_FILE ${GLSL_FILE})
    set(SPV_PATH "${CMAKE_CURRENT_SOURCE_DIR}/spv/${GLSL_BASE_FILE}.spv")
    add_custom_command(
        TARGET VulkanShaders
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/spv"
        COMMAND ${GLSLANG} -V ${GLSL_PATH} -o ${SPV_PATH})
endforeach(GLSL_SOURCE_ITEM)
