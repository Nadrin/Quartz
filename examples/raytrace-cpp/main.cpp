/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <QGuiApplication>
#include <QVulkanInstance>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

#include <Qt3DRaytrace/qcolorspace.h>
#include <Qt3DRaytrace/qrendersettings.h>
#include <Qt3DRaytrace/qcamera.h>
#include <Qt3DRaytrace/qmaterial.h>
#include <Qt3DRaytrace/qmesh.h>
#include <Qt3DRaytrace/qdistantlight.h>

#include <Qt3DRaytraceExtras/qt3dwindow.h>
#include <Qt3DRaytraceExtras/qfirstpersoncameracontroller.h>

static Qt3DCore::QEntity *createScene()
{
    auto *rootEntity = new Qt3DCore::QEntity;

    auto *camera = new Qt3DRaytrace::QCamera(rootEntity);
    camera->setPosition({0.0f, 0.0f, 4.0f});
    camera->setExposure(1.0f);
    camera->setFieldOfView(60.0f);

    auto *cameraController = new Qt3DRaytraceExtras::QFirstPersonCameraController(rootEntity);
    cameraController->setCamera(camera);
    cameraController->setMoveSpeed(2.0f);
    cameraController->setLookSpeed(180.0f);

    auto *renderSettings = new Qt3DRaytrace::QRenderSettings;
    renderSettings->setCamera(camera);
    renderSettings->setSkyColor(Qt3DRaytrace::to_sRgb(QColor::fromRgbF(0.8f, 0.9f, 1.0f)));
    renderSettings->setSkyIntensity(0.5f);
    rootEntity->addComponent(renderSettings);

    auto *sunEntity = new Qt3DCore::QEntity(rootEntity);
    {
        auto *sunTransform = new Qt3DCore::QTransform;
        sunTransform->setRotationX(-5.0f);
        sunTransform->setRotationZ(20.0f);

        auto *sunLight = new Qt3DRaytrace::QDistantLight;
        sunLight->setColor(Qt3DRaytrace::to_sRgb(QColor::fromRgbF(1.0f, 0.9f, 0.8f)));
        sunLight->setIntensity(1.0f);

        sunEntity->addComponent(sunTransform);
        sunEntity->addComponent(sunLight);
    }

    auto *monkeyEntity = new Qt3DCore::QEntity(rootEntity);
    {
        auto *monkeyMaterial = new Qt3DRaytrace::QMaterial;
        monkeyMaterial->setAlbedo(QColor("crimson"));
        monkeyMaterial->setRoughness(0.5f);

        auto *monkeyMesh = new Qt3DRaytrace::QMesh;
        monkeyMesh->setSource(QUrl("qrc:/monkey.obj"));

        monkeyEntity->addComponent(monkeyMaterial);
        monkeyEntity->addComponent(monkeyMesh);
    }

    auto *groundEntity = new Qt3DCore::QEntity(rootEntity);
    {
        auto *groundTransform = new Qt3DCore::QTransform;
        groundTransform->setTranslation({0.0f, -1.0f, 0.0f});
        groundTransform->setScale(10.0f);

        auto *groundMaterial = new Qt3DRaytrace::QMaterial;
        groundMaterial->setAlbedo(QColor("white"));

        auto *groundMesh = new Qt3DRaytrace::QMesh;
        groundMesh->setSource(QUrl("qrc:/plane.obj"));

        groundEntity->addComponent(groundTransform);
        groundEntity->addComponent(groundMaterial);
        groundEntity->addComponent(groundMesh);
    }

    return rootEntity;
}

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QVulkanInstance vulkanInstance;
    vulkanInstance.setApiVersion(QVersionNumber(1, 1));
    if(!vulkanInstance.create()) {
        qFatal("Failed to create Vulkan instance: %x", vulkanInstance.errorCode());
    }

    Qt3DRaytraceExtras::Qt3DWindow window;
    window.setVulkanInstance(&vulkanInstance);
    window.setWidth(1024);
    window.setHeight(1024);

    Qt3DCore::QEntity *scene = createScene();
    window.setRootEntity(scene);
    window.show();

    return app.exec();
}
