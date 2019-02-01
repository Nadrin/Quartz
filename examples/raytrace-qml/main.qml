import QtQuick 2.2
import Qt3D.Core 2.0
import Qt3D.Input 2.0
import Qt3D.Raytrace 1.0

Entity {
    id: root
    components: [
        InputSettings {}
    ]

    Entity {
        id: monkey
        components: [
            Transform {
                translation: Qt.vector3d(0, 0, -2)
            },
            Mesh {
                source: "file:data/monkey.obj"
            }
        ]
    }
}
