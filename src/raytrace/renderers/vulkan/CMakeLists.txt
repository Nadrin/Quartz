# Sources
target_sources(${MODULE_NAME} PRIVATE
    renderers/vulkan/vkcommon.h
    renderers/vulkan/vkresources.h
    renderers/vulkan/device.cpp
    renderers/vulkan/device.h
    renderers/vulkan/commandbuffer.cpp
    renderers/vulkan/commandbuffer.h
    renderers/vulkan/renderer.cpp
    renderers/vulkan/renderer.h
    renderers/vulkan/shadermodule.cpp
    renderers/vulkan/shadermodule.h
    renderers/vulkan/initializers.h
    renderers/vulkan/descriptors.h
    renderers/vulkan/resourcebarrier.h
    renderers/vulkan/geometry.h
    renderers/vulkan/glsl.h
    renderers/vulkan/services/frameadvanceservice.cpp
    renderers/vulkan/services/frameadvanceservice.h
    renderers/vulkan/pipeline/pipeline.cpp
    renderers/vulkan/pipeline/pipeline.h
    renderers/vulkan/pipeline/graphicspipeline.cpp
    renderers/vulkan/pipeline/graphicspipeline.h
    renderers/vulkan/pipeline/computepipeline.cpp
    renderers/vulkan/pipeline/computepipeline.h
    renderers/vulkan/pipeline/raytracingpipeline.cpp
    renderers/vulkan/pipeline/raytracingpipeline.h
    renderers/vulkan/jobs/buildgeometryjob.cpp
    renderers/vulkan/jobs/buildgeometryjob.h
    renderers/vulkan/jobs/buildscenetlasjob.cpp
    renderers/vulkan/jobs/buildscenetlasjob.h
    renderers/vulkan/jobs/updatematerialsjob.cpp
    renderers/vulkan/jobs/updatematerialsjob.h
    renderers/vulkan/jobs/updateemittersjob.cpp
    renderers/vulkan/jobs/updateemittersjob.h
    renderers/vulkan/jobs/updateinstancebufferjob.cpp
    renderers/vulkan/jobs/updateinstancebufferjob.h
    renderers/vulkan/jobs/updaterenderparametersjob.cpp
    renderers/vulkan/jobs/updaterenderparametersjob.h
    renderers/vulkan/jobs/uploadtexturejob.cpp
    renderers/vulkan/jobs/uploadtexturejob.h
    renderers/vulkan/jobs/destroyexpiredresourcesjob.cpp
    renderers/vulkan/jobs/destroyexpiredresourcesjob.h
    renderers/vulkan/managers/commandbuffermanager.cpp
    renderers/vulkan/managers/commandbuffermanager.h
    renderers/vulkan/managers/descriptormanager.cpp
    renderers/vulkan/managers/descriptormanager.h
    renderers/vulkan/managers/scenemanager.cpp
    renderers/vulkan/managers/scenemanager.h
    renderers/vulkan/managers/sceneresourceset.h
    renderers/vulkan/managers/cameramanager.cpp
    renderers/vulkan/managers/cameramanager.h
)

# Shaders
target_sources(${MODULE_NAME} PRIVATE
    renderers/vulkan/shaders/lib/bindings.glsl
    renderers/vulkan/shaders/lib/shared.glsl
    renderers/vulkan/shaders/vulkan_shaders.qrc
)

set(VULKAN_LIBRARIES volk vma spirv_reflect PARENT_SCOPE)
