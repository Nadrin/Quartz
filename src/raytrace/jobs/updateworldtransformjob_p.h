/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>
#include <Qt3DCore/QAspectJob>

namespace Qt3DRaytrace {
namespace Raytrace {

class Entity;

class UpdateWorldTransformJob : public Qt3DCore::QAspectJob
{
public:
    virtual ~UpdateWorldTransformJob() = default;

    void setRoot(Entity *root)
    {
        m_rootEntity = root;
    }
    void run() override;

private:
    Entity *m_rootEntity = nullptr;
};

using UpdateWorldTransformJobPtr = QSharedPointer<UpdateWorldTransformJob>;

} // Raytrace
} // Qt3DRaytrace
