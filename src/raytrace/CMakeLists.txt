set(MODULE_NAME "Qt3DRaytrace")
set(MODULE_LIBNAME "Qt53DRaytrace")
set(MODULE_API ${QUARTZ_PUBLIC_API}/${MODULE_NAME})
set(MODULE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(MODULE_PATH ${MODULE_PATH} ${MODULE_BINARY_DIR} PARENT_SCOPE)

find_package(Qt5 COMPONENTS Core Gui 3DCore REQUIRED)
find_package(assimp REQUIRED)

if(WIN32)
    set(THIRDPARTY_PATH ${THIRDPARTY_PATH} ${ASSIMP_BINARY_DIR})
endif()

set(SOURCES_PRIVATE
    qt3draytrace_global_p.h
    qraytraceaspect.cpp
    qraytraceaspect_p.h
    frontend/qgeometryrenderer.cpp
    frontend/qgeometryrenderer_p.h
    frontend/qgeometry.cpp
    frontend/qgeometry_p.h
    frontend/qmesh.cpp
    frontend/qmesh_p.h
    frontend/qmaterial.cpp
    frontend/qmaterial_p.h
    frontend/qdistantlight.cpp
    frontend/qdistantlight_p.h
    frontend/qcamera.cpp
    frontend/qcamera_p.h
    frontend/qcameralens.cpp
    frontend/qcameralens_p.h
    frontend/qrendersettings.cpp
    frontend/qrendersettings_p.h
    frontend/qabstracttexture.cpp
    frontend/qabstracttexture_p.h
    frontend/qtexture.cpp
    frontend/qtexture_p.h
    frontend/qtextureimage.cpp
    frontend/qtextureimage_p.h
    frontend/qtextureimage.cpp
    frontend/qtextureimage_p.h
    backend/handles_p.h
    backend/types_p.h
    backend/abstractrenderer_p.h
    backend/managers_p.h
    backend/backendnode.cpp
    backend/backendnode_p.h
    backend/entity.cpp
    backend/entity_p.h
    backend/transform.cpp
    backend/transform_p.h
    backend/geometryrenderer.cpp
    backend/geometryrenderer_p.h
    backend/geometry.cpp
    backend/geometry_p.h
    backend/material.cpp
    backend/material_p.h
    backend/distantlight.cpp
    backend/distantlight_p.h
    backend/cameralens.cpp
    backend/cameralens_p.h
    backend/rendersettings.cpp
    backend/rendersettings_p.h
    backend/abstracttexture.cpp
    backend/abstracttexture_p.h
    backend/textureimage.cpp
    backend/textureimage_p.h
    jobs/updateworldtransformjob.cpp
    jobs/updateworldtransformjob_p.h
    jobs/loadgeometryjob.cpp
    jobs/loadgeometryjob_p.h
    jobs/loadtexturejob.cpp
    jobs/loadtexturejob_p.h
    io/meshimporter_p.h
    io/imageimporter_p.h
    io/defaultmeshimporter.cpp
    io/defaultmeshimporter_p.h
    utility/movingaverage.h
)

set(SOURCES_PUBLIC
    ${MODULE_API}/qt3draytrace_global.h
    ${MODULE_API}/qraytraceaspect.h
    ${MODULE_API}/qrendererinterface.h
    ${MODULE_API}/qgeometryrenderer.h
    ${MODULE_API}/qgeometry.h
    ${MODULE_API}/qgeometrydata.h
    ${MODULE_API}/qgeometryfactory.h
    ${MODULE_API}/qmesh.h
    ${MODULE_API}/qmaterial.h
    ${MODULE_API}/qdistantlight.h
    ${MODULE_API}/qcamera.h
    ${MODULE_API}/qcameralens.h
    ${MODULE_API}/qrendersettings.h
    ${MODULE_API}/qabstracttexture.h
    ${MODULE_API}/qtexture.h
    ${MODULE_API}/qtextureimage.h
    ${MODULE_API}/qtextureimagefactory.h
    ${MODULE_API}/qimagedata.h
)

add_library(${MODULE_NAME} ${SOURCES_PRIVATE} ${SOURCES_PUBLIC})
set_target_properties(${MODULE_NAME} PROPERTIES OUTPUT_NAME ${MODULE_LIBNAME})

target_include_directories(${MODULE_NAME}
    PUBLIC ${QUARTZ_PUBLIC_API}
    PRIVATE ${assimp_INCLUDE_DIRS}
)

target_compile_features(${MODULE_NAME} PUBLIC cxx_std_14)
target_compile_definitions(${MODULE_NAME} PRIVATE VK_NO_PROTOTYPES)
target_link_libraries(${MODULE_NAME} Qt5::Core Qt5::Gui Qt5::3DCorePrivate ${assimp_LIBRARIES})

add_subdirectory(renderers)
add_dependencies(${MODULE_NAME} RendererShaders)
target_link_libraries(${MODULE_NAME} ${RENDERER_LIBRARIES})

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${MODULE_NAME}
        PUBLIC QUARTZ_SHARED
        PRIVATE QT3DRAYTRACE_LIBRARY
    )
endif()
