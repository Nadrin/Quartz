/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include "importer.h"

#include <QDebug>
#include <QFileInfo>

#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>

static constexpr unsigned int ImportFlags =
        aiProcess_TransformUVCoords |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ValidateDataStructure |
        aiProcess_FindInvalidData |
        aiProcess_FindInstances |
        aiProcess_FixInfacingNormals;

Importer::Importer()
{
    Assimp::DefaultLogger::create("", Assimp::Logger::NORMAL);
}

bool Importer::importScene(const QString &path)
{
    if(!QFileInfo(path).exists()) {
        qCritical() << "Error: Cannot open source file:" << path;
        return false;
    }

    const aiScene *scene = m_importer.ReadFile(path.toUtf8().constData(), ImportFlags);
    if(!scene) {
        qCritical() << "Error: Failed to import source file:" << path;
        return false;
    }

    m_scene.root = processScene(scene);
    if(!m_scene.root) {
        qCritical() << "Errors encountered while importing the scene file.";
        return false;
    }
    return true;
}

Entity *Importer::processScene(const aiScene *scene)
{
    for(unsigned int meshIndex=0; meshIndex < scene->mNumMeshes; ++meshIndex) {
        const aiMesh *mesh = scene->mMeshes[meshIndex];
        MeshComponent meshComponent;
        meshComponent.name = QString::fromUtf8(mesh->mName.C_Str());
        meshComponent.mesh = mesh;
        m_scene.meshes.append(meshComponent);
    }

    for(unsigned int textureIndex=0; textureIndex < scene->mNumTextures; ++textureIndex) {
        const aiTexture *texture = scene->mTextures[textureIndex];
        TextureComponent textureComponent;
        textureComponent.name = QString::fromUtf8(texture->mFilename.C_Str());
        textureComponent.width = texture->mWidth;
        textureComponent.height = texture->mHeight;
        if(texture->mHeight == 0) {
            textureComponent.format = QString::fromLocal8Bit(texture->achFormatHint);
        }
        textureComponent.data = reinterpret_cast<const unsigned char*>(texture->pcData);
        m_scene.textures.append(textureComponent);
    }

    for(unsigned int materialIndex=0; materialIndex < scene->mNumMaterials; ++materialIndex) {
        const aiMaterial *material = scene->mMaterials[materialIndex];
        MaterialComponent materialComponent;

        aiString name;
        if(material->Get(AI_MATKEY_NAME, name) == AI_SUCCESS) {
            materialComponent.name = QString::fromUtf8(name.C_Str());
        }

        aiColor3D color;
        float value;
        if(material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
            materialComponent.albedo = Color{ color.r, color.g, color.b };
        }
        if(material->Get(AI_MATKEY_REFLECTIVITY, value) == AI_SUCCESS) {
            materialComponent.roughness = 1.0f - qBound(0.0f, value, 1.0f);
        }
        if(material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) {
            materialComponent.emission = Color{ color.r, color.g, color.b };
            materialComponent.emissionIntensity = materialComponent.emission.normalizeIntensity();
        }

        materialComponent.albedoTextureIndex = processTextureReference(material, aiTextureType_DIFFUSE);
        materialComponent.roughnessTextureIndex = processTextureReference(material, aiTextureType_SHININESS);

        m_scene.materials.append(materialComponent);
    }

    Entity *rootNode = processSceneNode(scene->mRootNode);
    if(!rootNode) {
        qCritical() << "Error: Imported scene must contain at least one mesh";
    }
    return rootNode;
}

Entity *Importer::processSceneNode(const aiNode *node)
{
    QScopedPointer<Entity> entity(new Entity);
    entity->name = QString::fromUtf8(node->mName.C_Str());

    if(!node->mTransformation.IsIdentity()) {
        aiVector3D position, rotation, scaling;
        node->mTransformation.Decompose(scaling, rotation, position);
        entity->transform.translation = Vector3D{ position.x, position.y, position.z };
        entity->transform.rotation = Vector3D{ rotation.x, rotation.y, rotation.z };
        entity->transform.scale = Vector3D { scaling.x, scaling.y, scaling.z };
    }

    if(node->mNumMeshes > 1) {
        for(unsigned int meshNodeIndex = 0; meshNodeIndex < node->mNumMeshes; ++meshNodeIndex) {
            QScopedPointer<Entity> meshEntity(new Entity);
            meshEntity->name = QString("%1_%2").arg(entity->name).arg(meshNodeIndex);
            meshEntity->parent = entity.get();
            if(processMeshReference(node->mMeshes[meshNodeIndex], meshEntity.get())) {
                entity->children.append(meshEntity.take());
            }
        }
    }
    else if(node->mNumMeshes == 1) {
        processMeshReference(node->mMeshes[0], entity.get());
    }

    for(unsigned int childIndex=0; childIndex < node->mNumChildren; ++childIndex) {
        Entity *childEntity = processSceneNode(node->mChildren[childIndex]);
        if(childEntity) {
            childEntity->parent = entity.get();
            entity->children.append(childEntity);
        }
    }

    // Skip empty nodes/sub-trees.
    if(entity->children.size() == 0 && entity->meshComponentIndex == -1) {
        return nullptr;
    }

    return entity.take();
}

int Importer::processTextureReference(const aiMaterial *material, aiTextureType type)
{
    if(material->GetTextureCount(type) == 0) {
        return -1;
    }

    aiString path;
    material->GetTexture(type, 0, &path);
    if(path.data[0] == '*') {
        int textureIndex = QString::fromLocal8Bit(&path.data[1]).toInt();
        if(textureIndex < 0 || textureIndex >= m_scene.textures.size()) {
            qWarning() << "Warning: Found invalid embedded texture reference; ignoring.";
            return -1;
        }
        m_scene.textures[textureIndex].refcount++;
        return textureIndex;
    }
    else {
        const QString texturePath = QString::fromUtf8(path.C_Str());
        int textureIndex = m_texturesByPath.value(texturePath, -1);
        if(textureIndex >= 0) {
            m_scene.textures[textureIndex].refcount++;
            return textureIndex;
        }
        else {
            TextureComponent textureComponent;
            textureComponent.name = texturePath;
            m_scene.textures.append(textureComponent);
            m_texturesByPath.insert(texturePath, m_scene.textures.size());
            return m_scene.textures.size();
        }
    }
}

bool Importer::processMeshReference(unsigned int meshIndex, Entity *entity)
{
    if(meshIndex >= static_cast<unsigned int>(m_scene.meshes.size())) {
        qWarning() << "Warning: Found invalid mesh reference in node:" << entity->name;
        return false;
    }
    entity->meshComponentIndex = int(meshIndex);

    MeshComponent &meshComponent = m_scene.meshes[entity->meshComponentIndex];

    unsigned int materialIndex = meshComponent.mesh->mMaterialIndex;
    if(materialIndex >= static_cast<unsigned int>(m_scene.materials.size())) {
        qWarning() << "Warning: Found invalid material reference in node:" << entity->name;
        return false;
    }
    entity->materialComponentIndex = int(materialIndex);

    MaterialComponent &materialComponent = m_scene.materials[entity->materialComponentIndex];
    meshComponent.refcount++;
    materialComponent.refcount++;

    return true;
}
