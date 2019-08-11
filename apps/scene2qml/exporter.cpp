/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include "exporter.h"

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QRegularExpression>

#include <QtMath>
#include <QDebug>

namespace qml {

static QString indent(int n)
{
    return QString(4 * n, ' ');
}

static QString idForName(const QString &name, const QString &defaultId)
{
    QStringList parts = name.split(QRegularExpression("[^a-zA-Z0-9]"), QString::SkipEmptyParts);
    if(parts.empty()) {
        return defaultId;
    }
    else {
        if(parts[0][0].isNumber()) {
            parts[0].insert(0, '_');
        }
        else {
            parts[0].replace(0, 1, parts[0][0].toLower());
        }
        for(int i=1; i<parts.size(); ++i) {
            parts[i].replace(0, 1, parts[i][0].toUpper());
        }
        return parts.join("");
    }
}

static QString idPrintable(const QString &id)
{
    return QString("id_%1").arg(id);
}

} // qml

Exporter::Exporter(const Scene &scene)
    : m_scene(scene)
    , m_colorspace(Colorspace::Linear)
    , m_numExportedEntities(0)
    , m_numExportedMeshes(0)
    , m_numExportedTextures(0)
{}

void Exporter::setPrefix(const QString &prefix)
{
    m_prefix = prefix;
}

void Exporter::setMeshDirectory(const QString &path)
{
    m_meshDirectory.setPath(path);
}

void Exporter::setTexturesDirectory(const QString &path)
{
    m_texturesDirectory.setPath(path);
}

void Exporter::setColorspace(Colorspace colorspace)
{
    m_colorspace = colorspace;
}

bool Exporter::exportMeshes()
{
    Q_ASSERT(m_rootDirectory.absolutePath().length() > 0);
    Q_ASSERT(m_meshDirectory.absolutePath().length() > 0);

    const QString absoluteBasePath = QString("%1/%2").arg(m_rootDirectory.absolutePath()).arg(m_meshDirectory.path());

    bool targetDirectory = false;
    bool status = true;
    for(const MeshComponent &mesh : m_scene.meshes) {
        if(mesh.refcount == 0) {
            continue;
        }
        if(!targetDirectory) {
            if(!QDir().mkpath(absoluteBasePath)) {
                qCritical() << "Error: Failed to create mesh target directory:" << absoluteBasePath;
                return false;
            }
            targetDirectory = true;
        }

        const QString targetPath = getMeshAbsolutePath(mesh);
        if(writeMeshFile(targetPath, mesh)) {
            ++m_numExportedMeshes;
        }
        else {
            qCritical() << "Error: Failed to create mesh file:" << targetPath;
            status = false;
        }
    }
    return status;
}

bool Exporter::exportTextures(const QString &basePath)
{
    Q_ASSERT(m_rootDirectory.absolutePath().length() > 0);
    Q_ASSERT(m_texturesDirectory.absolutePath().length() > 0);

    const QString absoluteBasePath = QString("%1/%2").arg(m_rootDirectory.absolutePath()).arg(m_texturesDirectory.path());

    bool targetDirectory = false;
    bool status = true;
    for(const TextureComponent &texture : m_scene.textures) {
        if(texture.refcount == 0) {
            continue;
        }
        if(!targetDirectory) {
            if(!QDir().mkpath(absoluteBasePath)) {
                qCritical() << "Error: Failed to create textures target directory:" << absoluteBasePath;
                return false;
            }
            targetDirectory = true;
        }

        const QString targetPath = getTextureAbsolutePath(texture);
        if(texture.data) {
            // TODO: Export embedded texture to file.
            qWarning() << "Warning: Exporting embedded textures is not yet supported; skipping:" << targetPath;
        }
        else {
            const QString sourcePath = QDir::cleanPath(basePath + QDir::separator() + texture.name);

            QFile::remove(targetPath);
            if(QFile::copy(sourcePath, targetPath)) {
                ++m_numExportedTextures;
            }
            else {
                qCritical() << "Error: Failed to copy texture file:" << sourcePath;
                status = false;
            }
        }
    }
    return status;
}

bool Exporter::exportQml(const QString &path, const QString &sceneName)
{
    QFile outputFile(path);
    if(!outputFile.open(QFile::WriteOnly | QFile::Truncate)) {
        qCritical() << "Error: Failed to open output file for writing:" << path;
        return false;
    }

    m_rootDirectory.setPath(QFileInfo(path).path());

    QTextStream out(&outputFile);
    writeQmlHeader(out, sceneName);

    for(const MeshComponent &mesh : m_scene.meshes) {
        if(mesh.refcount > 1) {
            writeQmlMesh(out, mesh, nullptr, 1);
        }
    }
    for(const TextureComponent &texture : m_scene.textures) {
        if(texture.refcount > 1) {
            writeQmlTexture(out, texture, nullptr, 1);
        }
    }
    for(const MaterialComponent &material : m_scene.materials) {
        if(material.refcount > 1) {
            writeQmlMaterial(out, material, nullptr, 1);
        }
    }

    writeQmlEntity(out, m_scene.root, 1);
    writeQmlFooter(out);
    return true;
}

void Exporter::writeQmlHeader(QTextStream &out, const QString &sceneName)
{
    out << "// Generated by "
        << QCoreApplication::applicationName() << " v"
        << QCoreApplication::applicationVersion() << '\n';

    out << "// Source file: " << sceneName << "\n\n";

    out << "import QtQuick 2.0\n";
    out << "import Qt3D.Core 2.0\n";
    out << "import Qt3D.Raytrace 1.0\n\n";
    out << "Entity {\n";
    out << qml::indent(1) << "id: root\n";
}

void Exporter::writeQmlFooter(QTextStream &out)
{
    out << "}\n";
}

void Exporter::writeQmlEntity(QTextStream &out, const Entity *entity, int depth)
{
    const QString id = getOrCreateEntityId(entity);
    QVarLengthArray<QString, 3> componentIds;

    out << qml::indent(depth) << "Entity {\n";
    out << qml::indent(depth+1) << "id: " << qml::idPrintable(id) << "\n";
    if(entity->name.length() > 0) {
        out << qml::indent(depth+1) << "objectName: \"" << entity->name << "\"\n";
    }

    if(!entity->transform.isIdentity()) {
        componentIds.append(writeQmlTransform(out, entity, depth+1));
    }

    if(entity->meshComponentIndex >= 0) {
        const MeshComponent &mesh = m_scene.meshes[entity->meshComponentIndex];
        Q_ASSERT(mesh.refcount > 0);
        if(mesh.refcount == 1) {
            componentIds.append(writeQmlMesh(out, mesh, entity, depth+1));
        }
        else {
            componentIds.append(getComponentId(&mesh));
        }
    }
    if(entity->materialComponentIndex >= 0) {
        const MaterialComponent &material = m_scene.materials[entity->materialComponentIndex];
        Q_ASSERT(material.refcount > 0);
        if(material.refcount == 1) {
            componentIds.append(writeQmlMaterial(out, material, entity, depth+1));
        }
        else {
            componentIds.append(getComponentId(&material));
        }
    }

    if(componentIds.size() > 0) {
        out << qml::indent(depth+1) << "components: [";
        for(int i=0; i<componentIds.size(); ++i) {
            out << qml::idPrintable(componentIds[i]);
            if(i+1 < componentIds.size()) {
                out << ", ";
            }
        }
        out << "]\n";
    }

    for(const Entity *childEntity : entity->children) {
        writeQmlEntity(out, childEntity, depth+1);
    }

    out << qml::indent(depth) << "}\n";
    ++m_numExportedEntities;
}

QString Exporter::writeQmlMesh(QTextStream &out, const MeshComponent &mesh, const Entity *parentEntity, int depth)
{
    const QString id = getOrCreateComponentId(&mesh, "mesh", parentEntity);
    out << qml::indent(depth) << "Mesh {\n";
    out << qml::indent(depth+1) << "id: " << qml::idPrintable(id) << "\n";
    if(mesh.name.length() > 0) {
        out << qml::indent(depth+1) << "objectName: \"" << mesh.name << "\"\n";
    }
    out << qml::indent(depth+1) << "source: \"file:" << getMeshLogicalPath(mesh, m_prefix) << "\"\n";
    out << qml::indent(depth) << "}\n";
    return id;
}

QString Exporter::writeQmlMaterial(QTextStream &out, const MaterialComponent &material, const Entity *parentEntity, int depth)
{
    auto resolveTextureId = [this, &out, &material, depth](int textureIndex) -> QString {
        if(textureIndex >= 0) {
            const TextureComponent &textureComponent = m_scene.textures[textureIndex];
            Q_ASSERT(textureComponent.refcount > 0);
            if(textureComponent.refcount == 1) {
                return writeQmlTexture(out, textureComponent, &material, depth+1);
            }
            else {
                return getComponentId(&textureComponent);
            }
        }
        return QString();
    };

    const QString id = getOrCreateComponentId(&material, "material", parentEntity);
    out << qml::indent(depth) << "Material {\n";
    out << qml::indent(depth+1) << "id: " << qml::idPrintable(id) << "\n";
    if(material.name.length() > 0) {
        out << qml::indent(depth+1) << "objectName: \"" << material.name << "\"\n";
    }

    QString albedoTextureId = resolveTextureId(material.albedoTextureIndex);
    QString roughnessTextureId = resolveTextureId(material.roughnessTextureIndex);
    QString metalnessTextureId = resolveTextureId(material.metalnessTextureIndex);

    if(albedoTextureId.length() > 0) {
        out << qml::indent(depth+1) << "albedoTexture: " << qml::idPrintable(albedoTextureId) << "\n";
    }
    else {
        out << qml::indent(depth+1) << "albedo: " << colorString(material.albedo) << "\n";
    }

    if(roughnessTextureId.length() > 0) {
        out << qml::indent(depth+1) << "roughnessTexture: " << qml::idPrintable(roughnessTextureId) << "\n";
    }
    else if(!qFuzzyCompare(material.roughness, 1.0f)) {
        out << qml::indent(depth+1) << "roughness: " << material.roughness << "\n";
    }

    if(metalnessTextureId.length() > 0) {
        out << qml::indent(depth+1) << "metalnessTexture: " << qml::idPrintable(metalnessTextureId) << "\n";
    }
    else if(!qFuzzyCompare(material.metalness, 0.0f)) {
        out << qml::indent(depth+1) << "metalness: " << material.metalness << "\n";
    }

    if(!material.emission.isBlack()) {
        out << qml::indent(depth+1) << "emission: " << colorString(material.emission) << "\n";
        out << qml::indent(depth+1) << "emissionIntensity: " << material.emissionIntensity << "\n";
    }

    out << qml::indent(depth) << "}\n";
    return id;
}

QString Exporter::writeQmlTexture(QTextStream &out, const TextureComponent &texture, const MaterialComponent *parentMaterial, int depth)
{
    const QString id = getOrCreateComponentId(&texture, "texture", parentMaterial);
    out << qml::indent(depth) << "Texture {\n";
    out << qml::indent(depth+1) << "id: " << qml::idPrintable(id) << "\n";
    out << qml::indent(depth+1) << "source: \"file:" << getTextureLogicalPath(texture, m_prefix) << "\"\n";
    out << qml::indent(depth) << "}\n";
    return id;
}

QString Exporter::writeQmlTransform(QTextStream &out, const Entity *entity, int depth)
{
    const QString id = getOrCreateTransformId(entity);
    const Transform &transform = entity->transform;

    out << qml::indent(depth) << "Transform {\n";
    out << qml::indent(depth+1) << "id: " << qml::idPrintable(id) << "\n";
    if(!transform.translation.isNull()) {
        const auto &t = transform.translation;
        out << qml::indent(depth+1) << "translation: Qt.vector3d(" << t.x << "," << t.y << "," << t.z << ")\n";
    }
    if(!transform.rotation.isNull()) {
        const auto &r = transform.rotation;
        if(!qFuzzyIsNull(r.x)) {
            out << qml::indent(depth+1) << "rotationX: " << qRadiansToDegrees(r.x) << "\n";
        }
        if(!qFuzzyIsNull(r.y)) {
            out << qml::indent(depth+1) << "rotationY: " << qRadiansToDegrees(r.y) << "\n";
        }
        if(!qFuzzyIsNull(r.z)) {
            out << qml::indent(depth+1) << "rotationZ: " << qRadiansToDegrees(r.z) << "\n";
        }
    }
    if(!transform.scale.isOne()) {
        const auto &s = transform.scale;
        if(s.componentsEqual()) {
            out << qml::indent(depth+1) << "scale: " << s.x << "\n";
        }
        else {
            out << qml::indent(depth+1) << "scale3D: Qt.vector3d(" << s.x << "," << s.y << "," << s.z << ")\n";
        }
    }
    out << qml::indent(depth) << "}\n";
    return id;
}

bool Exporter::writeMeshFile(const QString &targetPath, const MeshComponent &mesh) const
{
    const aiMesh *data = mesh.mesh;
    if(!data->HasPositions()) {
        return false;
    }

    QFile outputFile(targetPath);
    if(!outputFile.open(QFile::WriteOnly | QFile::Truncate)) {
        qCritical() << "Error: Failed to open output file for writing:" << targetPath;
        return false;
    }

    QTextStream out(&outputFile);

    bool hasNormals = data->HasNormals();
    bool hasUVs = data->HasTextureCoords(0);

    for(unsigned int i=0; i<data->mNumVertices; ++i) {
        const auto &v = data->mVertices[i];
        out << "v " << v.x << ' ' << v.y << ' ' << v.z << '\n';
    }
    if(hasNormals) {
        for(unsigned int i=0; i<data->mNumVertices; ++i) {
            const auto &n = data->mNormals[i];
            out << "vn " << n.x << ' ' << n.y << ' ' << n.z << '\n';
        }
    }
    if(hasUVs) {
        for(unsigned int i=0; i<data->mNumVertices; ++i) {
            const auto &t = data->mTextureCoords[0][i];
            out << "vt " << t.x << ' ' << t.y << '\n';
        }
    }

    Q_ASSERT(data->mFaces);
    for(unsigned int i=0; i<data->mNumFaces; ++i) {
        const auto &f = data->mFaces[i];
        out << "f ";
        for(unsigned int j=0; j<f.mNumIndices; j++) {
            const QString index = QString::number(f.mIndices[j] + 1);

            if(hasNormals && hasUVs) {
                out << index << '/' << index << '/' << index;
            }
            else if(hasUVs) {
                out << index << '/' << index;
            }
            else if(hasNormals) {
                out << index << "//" << index;
            }
            else {
                out << index;
            }

            if(j+1 < f.mNumIndices) {
                out << ' ';
            }
        }
        out << '\n';
    }

    return true;
}

QString Exporter::createUniqueId(const QString &id)
{
    auto it = m_identifiers.find(id);
    if(it == m_identifiers.end()) {
        m_identifiers.insert(id, 0);
        return id;
    }
    else {
        int &n = it.value();
        ++n;
        return QString("%1_%2").arg(id).arg(n);
    }
}

QString Exporter::createUniqueId(const QString &hint, const QString &defaultName)
{
    const QString id = qml::idForName(hint, defaultName);
    return createUniqueId(id);
}

QString Exporter::getEntityId(const Entity *entity) const
{
    Q_ASSERT(m_entityIds.contains(entity));
    return m_entityIds.value(entity);
}

QString Exporter::getOrCreateEntityId(const Entity *entity)
{
    auto it = m_entityIds.find(entity);
    if(it == m_entityIds.end()) {
        QString id = createUniqueId(entity->name, "entity");
        m_entityIds.insert(entity, id);
        return id;
    }
    else {
        return it.value();
    }
}

QString Exporter::getOrCreateTransformId(const Entity *entity)
{
    auto it = m_transformIds.find(entity);
    if(it == m_transformIds.end()) {
        const QString entityId = getOrCreateEntityId(entity);
        QString id = createUniqueId(QString("%1_transform").arg(entityId));
        m_transformIds.insert(entity, id);
        return id;
    }
    else {
        return it.value();
    }
}

QString Exporter::getComponentId(const Component *component) const
{
    Q_ASSERT(m_componentIds.contains(component));
    return m_componentIds.value(component);
}

QString Exporter::getOrCreateComponentId(const Component *component, const QString &defaultId, const Entity *parentEntity)
{
    auto it = m_componentIds.find(component);
    if(it == m_componentIds.end()) {
        QString id;
        if(parentEntity) {
            const QString entityId = getEntityId(parentEntity);
            id = createUniqueId(QString("%1_%2").arg(entityId).arg(defaultId));
        }
        else {
            id = createUniqueId(component->name, defaultId);
        }
        m_componentIds.insert(component, id);
        return id;
    }
    else {
        return it.value();
    }
}

QString Exporter::getOrCreateComponentId(const Component *component, const QString &defaultId, const Component *parentComponent)
{
    auto it = m_componentIds.find(component);
    if(it == m_componentIds.end()) {
        QString id;
        if(parentComponent) {
            const QString parentComponentId = getComponentId(parentComponent);
            id = createUniqueId(QString("%1_%2").arg(parentComponentId).arg(defaultId));
        }
        else {
            id = createUniqueId(component->name, defaultId);
        }
        m_componentIds.insert(component, id);
        return id;
    }
    else {
        return it.value();
    }
}

QString Exporter::getMeshLogicalPath(const MeshComponent &mesh, const QString &prefix) const
{
    const QString meshFileId = getComponentId(&mesh).replace(QRegularExpression("_mesh$"), "");

    QString logicalPath = QString("%1/%2.obj").arg(m_meshDirectory.path()).arg(meshFileId);
    if(prefix.length() > 0) {
        logicalPath.prepend(QString("%1/").arg(prefix));
    }
    return logicalPath;
}

QString Exporter::getMeshAbsolutePath(const MeshComponent &mesh) const
{
    return QString("%1/%2").arg(m_rootDirectory.absolutePath()).arg(getMeshLogicalPath(mesh));
}

QString Exporter::getTextureLogicalPath(const TextureComponent &texture, const QString &prefix) const
{
    QString logicalPath;
    if(texture.data) {
        const QString textureFileId = getComponentId(&texture).replace(QRegularExpression("_texture$"), "");
        logicalPath = QString("%1/%2.png").arg(m_texturesDirectory.path()).arg(textureFileId);
    }
    else {
        logicalPath = QString("%1/%2").arg(m_texturesDirectory.path()).arg(QFileInfo(texture.name).fileName());
    }

    if(prefix.length() > 0) {
        logicalPath.prepend(QString("%1/").arg(prefix));
    }
    return logicalPath;
}

QString Exporter::getTextureAbsolutePath(const TextureComponent &texture) const
{
    return QString("%1/%2").arg(m_rootDirectory.absolutePath()).arg(getTextureLogicalPath(texture));
}

QString Exporter::colorString(const Color &c) const
{
    switch(m_colorspace) {
    case Colorspace::Linear:
        return QString("Qt3DRaytrace.lrgba(%1,%2,%3)").arg(c.r).arg(c.g).arg(c.b);
    case Colorspace::sRGB:
        return QString("Qt3DRaytrace.srgba(%1,%2,%3)").arg(c.r).arg(c.g).arg(c.b);
    }
    Q_ASSERT(false);
    return QString();
}
