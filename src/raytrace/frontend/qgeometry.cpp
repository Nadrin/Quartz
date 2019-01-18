/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <frontend/qgeometry_p.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {

QGeometry::QGeometry(QNode *parent)
    : QNode(*new QGeometryPrivate, parent)
{}

const QGeometryData &QGeometry::data() const
{
    Q_D(const QGeometry);
    return d->m_data;
}

const QVector<QVertex> &QGeometry::vertices() const
{
    Q_D(const QGeometry);
    return d->m_data.vertices;
}

const QVector<QTriangle> &QGeometry::faces() const
{
    Q_D(const QGeometry);
    return d->m_data.faces;
}

void QGeometry::setData(const QGeometryData &geometryData)
{
    Q_D(QGeometry);
    d->m_data = geometryData;
    QNodePrivate::get(this)->notifyPropertyChange("data", QVariant::fromValue(d->m_data));
    emit dataChanged(d->m_data);
}

void QGeometry::clearData()
{
    Q_D(QGeometry);
    d->m_data.vertices.clear();
    d->m_data.vertices.shrink_to_fit();
    d->m_data.faces.clear();
    d->m_data.faces.shrink_to_fit();

    QNodePrivate::get(this)->notifyPropertyChange("data", QVariant::fromValue(d->m_data));
    emit dataChanged(d->m_data);
}

QGeometry::QGeometry(QGeometryPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{}

QNodeCreatedChangeBasePtr QGeometry::createNodeCreationChange() const
{
    Q_D(const QGeometry);
    auto creationChange = QNodeCreatedChangePtr<QGeometryData>::create(this);
    creationChange->data = d->m_data;
    return creationChange;
}

} // Qt3DRaytrace
