/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>
#include <Qt3DRaytrace/qgeometry.h>
#include <Qt3DRaytrace/qgeometryfactory.h>

#include <Qt3DCore/QComponent>

namespace Qt3DRaytrace {

class QGeometryRendererPrivate;

class QT3DRAYTRACESHARED_EXPORT QGeometryRenderer : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRaytrace::QGeometry* geometry READ geometry WRITE setGeometry NOTIFY geometryChanged)
public:
    explicit QGeometryRenderer(Qt3DCore::QNode *parent = nullptr);

    QGeometry *geometry() const;

    QGeometryFactoryPtr geometryFactory() const;
    void setGeometryFactory(const QGeometryFactoryPtr &factory);

public slots:
    void setGeometry(QGeometry *geometry);

signals:
    void geometryChanged(QGeometry *geometry);

protected:
    explicit QGeometryRenderer(QGeometryRendererPrivate &dd, Qt3DCore::QNode *parent = nullptr);
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;

private:
    Q_DECLARE_PRIVATE(QGeometryRenderer)
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // Qt3DRaytrace
