/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <frontend/qmaterial_p.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {

QMaterialPrivate::QMaterialPrivate()
{
    m_data.albedo = Qt::GlobalColor::gray;
}

QMaterial::QMaterial(Qt3DCore::QNode *parent)
    : QComponent(*new QMaterialPrivate, parent)
{}

QColor QMaterial::albedo() const
{
    Q_D(const QMaterial);
    return d->m_data.albedo;
}

void QMaterial::setAlbedo(const QColor &albedo)
{
    Q_D(QMaterial);
    if(d->m_data.albedo != albedo) {
        d->m_data.albedo = albedo;
        emit albedoChanged(albedo);
    }
}

QMaterial::QMaterial(QMaterialPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{}

QNodeCreatedChangeBasePtr QMaterial::createNodeCreationChange() const
{
    Q_D(const QMaterial);

    auto creationChange = QNodeCreatedChangePtr<QMaterialData>::create(this);
    creationChange->data = d->m_data;
    return creationChange;
}

} // Qt3DRaytrace
