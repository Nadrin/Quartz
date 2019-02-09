add_subdirectory(vulkan)

add_custom_target(RendererShaders)
add_dependencies(RendererShaders VulkanShaders)

set(RENDERER_LIBRARIES ${VULKAN_LIBRARIES} PARENT_SCOPE)
