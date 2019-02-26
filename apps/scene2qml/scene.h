/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <QString>
#include <QVector>

#include <assimp/scene.h>

struct Vector3D
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    bool isNull() const
    {
        return qFuzzyIsNull(x) && qFuzzyIsNull(y) && qFuzzyIsNull(z);
    }
    bool isOne() const
    {
        return qFuzzyCompare(x, 1.0f) && qFuzzyCompare(y, 1.0f) && qFuzzyCompare(z, 1.0f);
    }
    bool componentsEqual() const
    {
        return qFuzzyCompare(x, y) && qFuzzyCompare(x, z);
    }
};

struct Color
{
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;

    bool isBlack() const
    {
        return qFuzzyIsNull(r) && qFuzzyIsNull(g) && qFuzzyIsNull(b);
    }
    float normalizeIntensity()
    {
        float intensity = std::max({r, g, b});
        if(intensity > 1.0f) {
            r /= intensity;
            g /= intensity;
            b /= intensity;
            return intensity;
        }
        else {
            return 1.0f;
        }
    }
};

struct Transform
{
    Vector3D translation;
    Vector3D rotation;
    Vector3D scale = {1.0f, 1.0f, 1.0f};

    bool isIdentity() const
    {
        return translation.isNull() && rotation.isNull() && scale.isOne();
    }
};

struct Component
{
    QString name;
    unsigned int refcount = 0;
};

struct MeshComponent : Component
{
    const aiMesh *mesh = nullptr;
};

struct MaterialComponent : Component
{
    Color albedo = {1.0f, 1.0f, 1.0f};
    Color emission;
    float emissionIntensity = 0.0f;
    float roughness = 1.0f;
    float metalness = 0.0f;
    int albedoTextureIndex = -1;
    int roughnessTextureIndex = -1;
    int metalnessTextureIndex = -1;
};

struct TextureComponent final : Component
{
    QString format;
    unsigned int width = 0;
    unsigned int height = 0;
    const unsigned char *data = nullptr;
};

struct Entity
{
    ~Entity() { qDeleteAll(children); }

    QString name;
    Entity *parent = nullptr;
    QVector<Entity*> children;
    Transform transform;
    int meshComponentIndex = -1;
    int materialComponentIndex = -1;
};

struct Scene
{
    ~Scene() { delete root; }

    Entity *root = nullptr;
    QVector<MeshComponent> meshes;
    QVector<MaterialComponent> materials;
    QVector<TextureComponent> textures;
};
