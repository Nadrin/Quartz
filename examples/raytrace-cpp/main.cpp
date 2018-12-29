/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <QGuiApplication>
#include <QVulkanInstance>
#include <Qt3DRaytraceExtras/qt3dwindow.h>

#if QUARTZ_DEBUG
#include <QLoggingCategory>

static const char *logFilterRules = R"(
        qt.vulkan=true
        raytrace.aspect=true
        raytrace.vulkan=true
)";
#endif

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QVulkanInstance vulkanInstance;
    vulkanInstance.setApiVersion(QVersionNumber(1, 1));
#ifdef QUARTZ_DEBUG
    QLoggingCategory::setFilterRules(logFilterRules);
    vulkanInstance.setLayers(QByteArrayList() << "VK_LAYER_LUNARG_stdandard_validation");
#endif
    if(!vulkanInstance.create()) {
        qFatal("Failed to create Vulkan instance: %x", vulkanInstance.errorCode());
    }

    Qt3DRaytraceExtras::Qt3DWindow window;
    window.setVulkanInstance(&vulkanInstance);
    window.show();

    return app.exec();
}
