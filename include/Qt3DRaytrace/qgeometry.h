/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>
#include <Qt3DRaytrace/qgeometrydata.h>

#include <Qt3DCore/QNode>
#include <QVector>

namespace Qt3DRaytrace {

class QGeometryPrivate;

class QT3DRAYTRACESHARED_EXPORT QGeometry : public Qt3DCore::QNode
{
    Q_OBJECT
public:
    explicit QGeometry(Qt3DCore::QNode *parent = nullptr);

    const QGeometryData &data() const;
    const QVector<QVertex> &vertices() const;
    const QVector<QTriangle> &faces() const;

    void setData(const QGeometryData &geometryData);
    void clearData();

signals:
    void dataChanged(const QGeometryData &geometryData);

protected:
    explicit QGeometry(QGeometryPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QGeometry)
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // Qt3DRaytrace
