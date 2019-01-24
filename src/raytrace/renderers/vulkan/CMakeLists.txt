cmake_minimum_required(VERSION 3.8)

target_sources(${MODULE_NAME} PRIVATE
    renderers/vulkan/vkcommon.h
    renderers/vulkan/vkresources.h
    renderers/vulkan/device.cpp
    renderers/vulkan/device.h
    renderers/vulkan/commandbuffer.cpp
    renderers/vulkan/commandbuffer.h
    renderers/vulkan/renderer.cpp
    renderers/vulkan/renderer.h
    renderers/vulkan/frameadvanceservice.cpp
    renderers/vulkan/frameadvanceservice.h
    renderers/vulkan/shadermodule.cpp
    renderers/vulkan/shadermodule.h
    renderers/vulkan/initializers.h
    renderers/vulkan/descriptors.h
    renderers/vulkan/resourcebarrier.h
    renderers/vulkan/pipeline/pipeline.cpp
    renderers/vulkan/pipeline/pipeline.h
    renderers/vulkan/pipeline/graphicspipeline.cpp
    renderers/vulkan/pipeline/graphicspipeline.h
    renderers/vulkan/pipeline/computepipeline.cpp
    renderers/vulkan/pipeline/computepipeline.h
)

add_subdirectory(shaders)
set(VULKAN_LIBRARIES volk vma spirv_reflect PARENT_SCOPE)
