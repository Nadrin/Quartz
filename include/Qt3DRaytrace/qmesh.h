/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>
#include <Qt3DRaytrace/qgeometryrenderer.h>

#include <QUrl>

namespace Qt3DRaytrace {

class QMeshPrivate;

class QT3DRAYTRACESHARED_EXPORT QMesh : public QGeometryRenderer
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
public:
    explicit QMesh(Qt3DCore::QNode *parent = nullptr);

    enum Status {
        None = 0,
        Loading,
        Ready,
        Error,
    };
    Q_ENUM(Status)

    QUrl source() const;
    Status status() const;

public slots:
    void setSource(const QUrl &source);

signals:
    void sourceChanged(const QUrl &source);
    void statusChanged(Status status);

protected:
    explicit QMesh(QMeshPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QMesh)
};

} // Qt3DRaytrace
