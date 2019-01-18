/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <frontend/qmesh_p.h>
#include <io/defaultmeshimporter_p.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {

// TODO: Implement status change.
// TODO: Make mesh importer configurable.

QMesh::QMesh(QNode *parent)
    : QGeometryRenderer(*new QMeshPrivate, parent)
{}

QMesh::QMesh(QMeshPrivate &dd, QNode *parent)
    : QGeometryRenderer(dd, parent)
{}

QUrl QMesh::source() const
{
    Q_D(const QMesh);
    return d->m_source;
}

QMesh::Status QMesh::status() const
{
    Q_D(const QMesh);
    return d->m_status;
}

void QMesh::setSource(const QUrl &source)
{
    Q_D(QMesh);
    if(d->m_source != source) {
        d->m_source = source;
        setGeometryFactory(QGeometryFactoryPtr(new MeshLoader(this)));
        emit sourceChanged(source);
    }
}

MeshLoader::MeshLoader(const QMesh *mesh)
    : m_importer(new Raytrace::DefaultMeshImporter)
    , m_source(mesh->source())
{}

QGeometry *MeshLoader::create()
{
    Q_ASSERT(m_importer);

    if(m_source.isEmpty()) {
        qCWarning(logAspect) << Q_FUNC_INFO << "Mesh source path is empty";
        return nullptr;
    }

    QGeometryData geometryData;
    if(!m_importer->import(m_source, geometryData)) {
        qCWarning(logAspect) << Q_FUNC_INFO << "Failed to import geometry data";
        return nullptr;
    }

    QGeometry *geometry = new QGeometry;
    geometry->setData(geometryData);
    return geometry;
}

} // Qt3DRaytrace
