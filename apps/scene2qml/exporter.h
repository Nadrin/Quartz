/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <QString>
#include <QMap>
#include <QDir>

#include "scene.h"

class QTextStream;

enum class Colorspace
{
    Linear,
    sRGB,
};

class Exporter
{
public:
    explicit Exporter(const Scene &scene);

    void setPrefix(const QString &prefix);
    void setMeshDirectory(const QString &path);
    void setTexturesDirectory(const QString &path);
    void setColorspace(Colorspace colorspace);

    bool exportQml(const QString &path, const QString &sceneName);
    bool exportMeshes();
    bool exportTextures(const QString &basePath);

    int numExportedEntities() const { return m_numExportedEntities; }
    int numExportedMeshes() const { return m_numExportedMeshes; }
    int numExportedTextures() const { return m_numExportedTextures; }

private:
    void writeQmlHeader(QTextStream &out, const QString &sceneName);
    void writeQmlFooter(QTextStream &out);
    void writeQmlEntity(QTextStream &out, const Entity *entity, int depth);
    QString writeQmlMesh(QTextStream &out, const MeshComponent &mesh, const Entity *parentEntity, int depth);
    QString writeQmlMaterial(QTextStream &out, const MaterialComponent &material, const Entity *parentEntity, int depth);
    QString writeQmlTexture(QTextStream &out, const TextureComponent &texture, const MaterialComponent *parentMaterial, int depth);
    QString writeQmlTransform(QTextStream &out, const Entity *entity, int depth);

    bool writeMeshFile(const QString &targetPath, const MeshComponent &mesh) const;

    QString createUniqueId(const QString &id);
    QString createUniqueId(const QString &hint, const QString &defaultName);

    QString getEntityId(const Entity *entity) const;
    QString getOrCreateEntityId(const Entity *entity);
    QString getOrCreateTransformId(const Entity *entity);

    QString getComponentId(const Component *component) const;
    QString getOrCreateComponentId(const Component *component, const QString &defaultId, const Entity *parentEntity);
    QString getOrCreateComponentId(const Component *component, const QString &defaultId, const Component *parentComponent);

    QString getMeshLogicalPath(const MeshComponent &mesh, const QString &prefix="") const;
    QString getMeshAbsolutePath(const MeshComponent &mesh) const;
    QString getTextureLogicalPath(const TextureComponent &texture, const QString &prefix="") const;
    QString getTextureAbsolutePath(const TextureComponent &texture) const;

    QString colorString(const Color &c) const;

    const Scene &m_scene;

    QMap<QString, int> m_identifiers;
    QMap<const Entity*, QString> m_entityIds;
    QMap<const Entity*, QString> m_transformIds;
    QMap<const Component*, QString> m_componentIds;

    QDir m_rootDirectory;
    QDir m_meshDirectory;
    QDir m_texturesDirectory;

    QString m_prefix;

    Colorspace m_colorspace;

    int m_numExportedEntities;
    int m_numExportedMeshes;
    int m_numExportedTextures;
};
