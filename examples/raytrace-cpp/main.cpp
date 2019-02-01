/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <QGuiApplication>
#include <QVulkanInstance>
#include <Qt3DRaytraceExtras/qt3dwindow.h>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRaytrace/qmesh.h>

#if QUARTZ_DEBUG
#include <QLoggingCategory>

static const char *logFilterRules = R"(
        qt.vulkan=true
        raytrace.aspect=true
        raytrace.import=true
        raytrace.vulkan=true
)";
#endif

static Qt3DCore::QEntity *createScene()
{
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity;

    Qt3DCore::QEntity *monkeyEntity = new Qt3DCore::QEntity(rootEntity);

    Qt3DCore::QTransform *monkeyTransform = new Qt3DCore::QTransform;
    monkeyTransform->setTranslation({0.0f, 0.0f, -2.0f});

    Qt3DRaytrace::QMesh *monkeyMesh = new Qt3DRaytrace::QMesh;
    monkeyMesh->setSource(QUrl::fromLocalFile("data/monkey.obj"));

    monkeyEntity->addComponent(monkeyTransform);
    monkeyEntity->addComponent(monkeyMesh);

    return rootEntity;
}

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QVulkanInstance vulkanInstance;
    vulkanInstance.setApiVersion(QVersionNumber(1, 1));
#ifdef QUARTZ_DEBUG
    QLoggingCategory::setFilterRules(logFilterRules);
    vulkanInstance.setLayers(QByteArrayList() << "VK_LAYER_LUNARG_standard_validation");
#endif
    if(!vulkanInstance.create()) {
        qFatal("Failed to create Vulkan instance: %x", vulkanInstance.errorCode());
    }

    Qt3DRaytraceExtras::Qt3DWindow window;
    window.setVulkanInstance(&vulkanInstance);

    Qt3DCore::QEntity *scene = createScene();
    window.setRootEntity(scene);
    window.show();

    return app.exec();
}
