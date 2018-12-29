cmake_minimum_required(VERSION 3.8)

set(VULKAN_SOURCES_PRIVATE
    renderers/vulkan/vkcommon.h
    renderers/vulkan/qvulkanrenderer.cpp
    renderers/vulkan/qvulkanrenderer_p.h
    renderers/vulkan/vulkanframeadvanceservice.cpp
    renderers/vulkan/vulkanframeadvanceservice.h
    renderers/vulkan/shadermodule.cpp
    renderers/vulkan/shadermodule.h
    renderers/vulkan/pipeline/pipeline.cpp
    renderers/vulkan/pipeline/pipeline.h
    renderers/vulkan/pipeline/graphicspipeline.cpp
    renderers/vulkan/pipeline/graphicspipeline.h
    renderers/vulkan/pipeline/computepipeline.cpp
    renderers/vulkan/pipeline/computepipeline.h
)

set(VULKAN_SOURCES_PUBLIC
    ${MODULE_API}/qvulkanrenderer.h
)

add_subdirectory(shaders)

target_sources(${MODULE_NAME}
    PRIVATE ${VULKAN_SOURCES_PRIVATE}
    PUBLIC ${VULKAN_SOURCES_PUBLIC}
)
set(VULKAN_LIBRARIES volk vma spirv_reflect PARENT_SCOPE)
