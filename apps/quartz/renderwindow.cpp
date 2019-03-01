/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include "renderwindow.h"

#include <QVulkanInstance>
#include <QScopedPointer>

#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QDir>

RenderWindow::RenderWindow()
{
    setTitle("Quartz");
}

QVulkanInstance *RenderWindow::createDefaultVulkanInstance()
{
    QScopedPointer<QVulkanInstance> vkInstance(new QVulkanInstance);
    vkInstance->setApiVersion(QVersionNumber(1, 1));
#ifdef QUARTZ_DEBUG
    vkInstance->setLayers(QByteArrayList() << "VK_LAYER_LUNARG_standard_validation");
#endif
    if(!vkInstance->create()) {
        QMessageBox::critical(nullptr, tr("Quartz"), QString("%1: %2")
                              .arg(tr("Failed to create Vulkan instance"))
                              .arg(vkInstance->errorCode()));
        return nullptr;
    }
    return vkInstance.take();
}

bool RenderWindow::chooseSourceFile()
{
    QFileDialog dialog(nullptr, tr("Open QML file"));
    dialog.setNameFilter("QML Files (*.qml)");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setDirectory(QDir::currentPath());
    if(dialog.exec()) {
        const QString path = dialog.selectedFiles().at(0);
        return setSourceFile(path);
    }
    return false;
}

bool RenderWindow::setSourceFile(const QString &path)
{
    const QString absolutePath = QDir::current().absoluteFilePath(path);
    {
        QFile sceneFile(absolutePath);
        if(!sceneFile.open(QFile::ReadOnly)) {
            QMessageBox::critical(nullptr, tr("Quartz"), QString("%1: %2")
                                  .arg(tr("Cannot open input file"))
                                  .arg(absolutePath));
            return false;
        }
    }

    QDir::setCurrent(QFileInfo(absolutePath).absolutePath());

    setSource(QUrl::fromLocalFile(absolutePath));
    setSceneName(QFileInfo(absolutePath).fileName());
    return true;
}

void RenderWindow::setSceneName(const QString &name)
{
    m_sceneName = name;
    if(m_sceneName.length() > 0) {
        setTitle(QString("%1 - %2").arg(m_sceneName).arg(title()));
    }
}
