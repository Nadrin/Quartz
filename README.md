# Quartz

&copy; 2018 - 2019 Michał Siejak ([@Nadrin](https://twitter.com/Nadrin))

A physically based GPU path tracing renderer with a declarative ES7-like scene description language.

Powered by Qt 3D, NVIDIA RTX & Vulkan.

![Images rendered by Quartz](https://raw.githubusercontent.com/Nadrin/Quartz/master/doc/renders.jpg)

## Overview

Quartz consists of several components:

* At its core is a raytracing aspect for [Qt 3D](https://doc.qt.io/qt-5/qt3d-index.html) implemented by `Qt3DRaytrace` module. It provides both C++ and [QML](https://en.wikipedia.org/wiki/QML) APIs for use in Qt 3D based applications and is a drop-in replacement for `Qt3DRender` (rasterization-based rendering aspect). While not (yet) on par with `Qt3DRender` features, many types are named the same and provide similar functionality, thus making the two aspects somewhat analogous.
* A standalone renderer application that can load and render arbitrary scenes in Qt 3D [QML](https://en.wikipedia.org/wiki/QML) format. It provides live, progressive preview and can save to many LDR and HDR image formats.
* An "extras" module (`Qt3DRaytraceExtras`) providing various auxiliary utilities.
* A command-line tool for assisting in conversion of complex 3D scenes to QML defined hierarchies (`scene2qml`).

## Features

This project is still a work in progress. Items without a check mark are planned but not yet implemented at this time.

- [x] Unidirectional path tracing
- [x] Multiple importance sampling (MIS)
- [x] Next event estimation
- [x] Physically based BRDF with metalness workflow (based on Unreal Engine 4 material model)
- [x] Texture mapping (albedo, roughness & metalness maps)
- [x] Instancing
- [x] Distant (directional) lights
- [x] Area lights (emissive triangle meshes)
- [x] IBL environment probes
- [x] Thin-lens model with depth of field
- [x] Reinhard tone-mapping operator
- [x] Interactive camera controller
- [ ] Wavefront based rendering architecture
- [ ] Normal mapping
- [ ] Texture LOD selection via ray differentials
- [ ] Transmission (non-opaque BSDF)
- [ ] Stratified and low-discrepancy sampling
- [ ] Denoising
- [ ] Importance sampling of IBL probes
- [ ] Gaussian reconstruction filter
- [ ] Bloom

## System requirements

Quartz requires an NVIDIA RTX-series GPU with support for the following Vulkan extensions:

- `VK_KHR_swapchain`
- `VK_NV_ray_tracing`
- `VK_EXT_descriptor_indexing`

## Usage

To render with the standalone renderer run `Quartz.exe` (or `./quartz` on Linux) and select a QML scene file in the open dialog. Alternatively you can use the command line:

```
Quartz.exe [-x <viewport_width>] [-y <viewport_height>] [path_to_qml_file]
```

If the opened QML file contains an instance of `FirstPersonCameraController` the camera can be controlled interactively. Press and hold either left or right mouse button and drag the mouse around to rotate the view. Use the usual `W`, `S`, `A`, and `D` for movement. `Q` and `E` move up and down respectively.

Press `F2` to save an image file. Saving to HDR (Radiance) format writes a raw floating-point image in linear space. Saving to any other format writes a tone-mapped, gamma corrected image.

## Project structure

Path | Description
-----|------------
`/3rdparty` | Third party libraries
`/apps/quartz` | Standalone renderer application
`/apps/scene2qml` | 3D scene to QML conversion tool
`/cmake` | Local CMake modules
`/doc` | Documentation (WIP)
`/examples/assets` | Assets used by example projects
`/examples/raytrace-cpp` | C++ API usage example
`/examples/raytrace-qml` | QML API usage example
`/include` | Public C++ API include files
`/src/extras` | Extras library (`Qt3DRaytraceExtras`)
`/src/qml` | QML plugins
`/src/raytrace` | Raytracing aspect library (`Qt3DRaytrace`)
`/src/raytrace/renderers/vulkan` | Raytracing aspect Vulkan renderer

## Third party libraries

This project makes use of the following open source libraries:

- [Qt 5.12](https://www.qt.io/)
- [Open Asset Import Library](http://www.assimp.org/)
- [Vulkan Memory Allocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
- [SPIRV-Reflect](https://github.com/chaoticbob/SPIRV-Reflect)
- [volk](https://github.com/zeux/volk)
- [stb libraries](https://github.com/nothings/stb)
