/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <io/defaultmeshimporter_p.h>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/DefaultLogger.hpp>

#include <QFile>
#include <QFileInfo>

#include <QMutex>
#include <QMutexLocker>

namespace Qt3DRaytrace {

Q_LOGGING_CATEGORY(logImport, "raytrace.import")

namespace Raytrace {

static constexpr unsigned int ImportFlags =
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_SortByPType |
        aiProcess_PreTransformVertices |
        aiProcess_GenNormals |
        aiProcess_GenUVCoords |
        aiProcess_OptimizeMeshes |
        aiProcess_Debone |
        aiProcess_ValidateDataStructure;

static constexpr QVector3D TangentGenUp{0.0f, 1.0f, 0.0f};
static constexpr QVector3D TangentGenRight{1.0f, 0.0f, 0.0f};
static constexpr float     TangentGenLengthThreshold = 0.001f;

class LogStream final : public Assimp::LogStream
{
public:
    static void initialize()
    {
        QMutexLocker lock(&LoggerInitMutex);
        if(Assimp::DefaultLogger::isNullLogger()) {
            Assimp::DefaultLogger::create("", Assimp::Logger::NORMAL);
            Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn);
        }
    }
    void write(const char *message) override
    {
        qCWarning(logImport) << message;
    }

private:
    LogStream() = default;
    static QMutex LoggerInitMutex;
};

QMutex LogStream::LoggerInitMutex;

static bool importScene(const aiScene *scene, QGeometryData &data)
{
    int totalNumVertices = 0;
    int totalNumFaces = 0;

    QVector<quint32> meshBaseIndices;
    meshBaseIndices.reserve(int(scene->mNumMeshes));
    for(int i=0; i<int(scene->mNumMeshes); ++i) {
        const aiMesh *mesh = scene->mMeshes[i];
        if(mesh->HasPositions() && mesh->HasNormals()) {
            meshBaseIndices.append(quint32(totalNumVertices));
            totalNumVertices += int(mesh->mNumVertices);
            totalNumFaces += int(mesh->mNumFaces);
        }
    }
    if(totalNumVertices == 0 || totalNumFaces == 0) {
        return false;
    }

    data.vertices.resize(totalNumVertices);
    data.faces.resize(totalNumFaces);

    for(int i=0, vertexIndex=0, triangleIndex=0; i<int(scene->mNumMeshes); ++i) {
        const aiMesh *mesh = scene->mMeshes[i];
        if(!mesh->HasPositions() || !mesh->HasNormals()) {
            continue;
        }
        for(int j=0; j<int(mesh->mNumVertices); ++j, ++vertexIndex) {
            auto &vertex = data.vertices[vertexIndex];

            vertex.position = { mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z };
            vertex.normal = { mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z };
            if(mesh->HasTextureCoords(0)) {
                vertex.texcoord = { mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y };
            }
            if(mesh->HasTangentsAndBitangents()) {
                vertex.tangent = { mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z };
            }
            else {
                vertex.tangent = QVector3D::crossProduct(TangentGenUp, vertex.normal);
                if(vertex.tangent.lengthSquared() < TangentGenLengthThreshold) {
                    vertex.tangent = QVector3D::crossProduct(TangentGenRight, vertex.normal);
                }
                vertex.tangent.normalize();
            }
        }
        for(int j=0; j<int(mesh->mNumFaces); ++j, ++triangleIndex) {
            auto &triangle = data.faces[triangleIndex];
            triangle.vertices[0] = meshBaseIndices[i] + mesh->mFaces[j].mIndices[0];
            triangle.vertices[1] = meshBaseIndices[i] + mesh->mFaces[j].mIndices[1];
            triangle.vertices[2] = meshBaseIndices[i] + mesh->mFaces[j].mIndices[2];
        }
    }
    return true;
}

bool DefaultMeshImporter::import(const QUrl &url, QGeometryData &data)
{
    LogStream::initialize();

    qCInfo(logImport) << "Loading mesh:" << url.toString();

    const aiScene *scene = nullptr;
    Assimp::Importer importer;
    {
        QFile sceneFile(url.path());
        if(!sceneFile.open(QFile::ReadOnly)) {
            qCCritical(logImport) << "Cannot open mesh file:" << url;
            return false;
        }

        const QByteArray sceneData = sceneFile.readAll();
        const QByteArray sceneHint = QFileInfo(url.path()).completeSuffix().toUtf8();
        scene = importer.ReadFileFromMemory(sceneData.data(), size_t(sceneData.size()), ImportFlags, sceneHint.data());
    }

    bool result = false;
    if(scene && scene->HasMeshes()) {
        result = importScene(scene, data);
    }
    if(!result) {
        qCCritical(logImport) << "Failed to import mesh from file:" << url;
    }
    return result;
}

} // Raytrace
} // Qt3DRaytrace
