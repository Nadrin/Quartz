/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>

#include <Qt3DCore/QComponent>
#include <QColor>
#include <QVector3D>

namespace Qt3DRaytrace {

class QDistantLightPrivate;

class QT3DRAYTRACESHARED_EXPORT QDistantLight : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(float intensity READ intensity WRITE setIntensity NOTIFY intensityChanged)
    Q_PROPERTY(QVector3D direction READ direction WRITE setDirection NOTIFY directionChanged)
public:
    explicit QDistantLight(Qt3DCore::QNode *parent = nullptr);

    QColor color() const;
    float intensity() const;
    QVector3D direction() const;

public slots:
    void setColor(const QColor &color);
    void setIntensity(float intensity);
    void setDirection(const QVector3D &direction);

signals:
    void colorChanged(const QColor &color);
    void intensityChanged(float intensity);
    void directionChanged(const QVector3D &direction);

protected:
    explicit QDistantLight(QDistantLightPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QDistantLight)
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // Qt3DRaytrace
