/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

import QtQuick 2.2
import Qt3D.Core 2.0
import Qt3D.Input 2.0
import Qt3D.Raytrace 1.0
import Qt3D.RaytraceExtras 1.0

Entity {
    id: root
    components: [
        InputSettings {},
        RenderSettings {
            camera: camera
            skyColor: Qt3DRaytrace.lrgba(0.8, 0.9, 1.0)
            skyIntensity: 0.5
        }
    ]

    Camera {
        id: camera
        position: Qt.vector3d(0, 0, 4)
        exposure: 1.0
        fieldOfView: 60
    }
    FirstPersonCameraController {
        camera: camera
        moveSpeed: 2
        lookSpeed: 180
    }

    // Sun
    Entity {
        Transform {
            id: sunTransform
            rotationX: -5
            rotationZ: 20
        }
        DistantLight {
            id: sunLight
            color: Qt3DRaytrace.lrgba(1.0, 0.9, 0.8)
            intensity: 1.0
        }
        components: [ sunTransform, sunLight ]
    }

    // Monkey
    Entity {
        Material {
            id: monkeyMaterial
            albedo: "crimson"
            roughness: 0.5
        }
        Mesh {
            id: monkeyMesh
            source: "monkey.obj"
        }
        components: [ monkeyMaterial, monkeyMesh ]
    }

    // Ground
    Entity {
        Transform {
            id: groundTransform
            translation: Qt.vector3d(0, -1, 0)
            scale: 10
        }
        Material {
            id: groundMaterial
            albedo: "white"
        }
        Mesh {
            id: groundMesh
            source: "plane.obj"
        }
        components: [ groundTransform, groundMaterial, groundMesh ]
    }
}
