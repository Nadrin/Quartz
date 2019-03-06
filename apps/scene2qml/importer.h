/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <QString>
#include <QVector>
#include <QMap>
#include <QScopedPointer>

#include "scene.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

class Importer
{
public:
    Importer();

    void setImportFlag(unsigned int flag);
    bool importScene(const QString &path);

    const Scene &scene() const
    {
        return m_scene;
    }

private:
    Entity *processScene(const aiScene *scene);
    Entity *processSceneNode(const aiNode *node);

    int processTextureReference(const aiMaterial *material, aiTextureType type);
    bool processMeshReference(unsigned int meshIndex, Entity *entity);

    Scene m_scene;
    Assimp::Importer m_importer;
    QMap<QString, int> m_texturesByPath;
    unsigned int m_importFlags;
};
