/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <jobs/updateworldtransformjob_p.h>
#include <backend/entity_p.h>
#include <backend/transform_p.h>

namespace Qt3DRaytrace {
namespace Raytrace {

static void updateWorldTransform(Entity *entity, const Matrix4x4 &parentTransformMatrix)
{
    Matrix4x4 worldTransformMatrix = parentTransformMatrix;
    const Transform *transform = entity->transformComponent();
    if(transform) {
        worldTransformMatrix *= transform->transformMatrix();
    }
    entity->worldTransformMatrix = worldTransformMatrix;

    for(Entity *childEntity : entity->children()) {
        updateWorldTransform(childEntity, worldTransformMatrix);
    }
}

void UpdateWorldTransformJob::run()
{
    Q_ASSERT(m_rootEntity);

    Matrix4x4 parentTransformMatrix;
    Entity *parent = m_rootEntity->parent();
    if(parent) {
        parentTransformMatrix = parent->worldTransformMatrix;
    }
    updateWorldTransform(m_rootEntity, parentTransformMatrix);
}

} // Raytrace
} // Qt3DRaytrace
