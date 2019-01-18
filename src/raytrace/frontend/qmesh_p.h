/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qmesh.h>
#include <frontend/qgeometryrenderer_p.h>
#include <io/meshimporter_p.h>

#include <QScopedPointer>

namespace Qt3DRaytrace {

class QMeshPrivate : public QGeometryRendererPrivate
{
public:
    Q_DECLARE_PUBLIC(QMesh)

    QUrl m_source;
    QMesh::Status m_status = QMesh::None;
};

class MeshLoader final : public QGeometryFactory
{
public:
    explicit MeshLoader(const QMesh *mesh);

    QGeometry *create() override;

private:
    QScopedPointer<Raytrace::MeshImporter> m_importer;
    QUrl m_source;
};

} // Qt3DRaytrace
