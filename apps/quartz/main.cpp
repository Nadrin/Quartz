/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <QApplication>
#include <QCommandLineParser>
#include <QTextStream>
#include <QScopedPointer>

#include "renderwindow.h"

namespace Config {
static constexpr int DefaultViewportWidth  = 1280;
static constexpr int DefaultViewportHeight = 720;
} // Config

static void parseOptions(int &viewportWidth, int &viewportHeight, QString &sceneFilePath)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("Quartz Standalone Renderer");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption widthOption({"x", "sizex", "width"}, "Initial viewport width.", "px",
                                   QString::number(Config::DefaultViewportWidth));
    QCommandLineOption heightOption({"y", "sizey", "height"}, "Initial viewport height.", "px",
                                    QString::number(Config::DefaultViewportHeight));
    parser.addOptions({widthOption, heightOption});
    parser.addPositionalArgument("scene", "QML scene file path");

    parser.process(*QApplication::instance());
    viewportWidth = parser.value(widthOption).toInt();
    viewportHeight = parser.value(heightOption).toInt();
    if(viewportWidth <= 0 || viewportHeight <= 0) {
        QTextStream(stderr) << "Error: Invalid viewport dimensions\n";
        parser.showHelp(1);
    }

    if(parser.positionalArguments().size() > 0) {
        sceneFilePath = parser.positionalArguments().at(0);
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("Quartz");
    QApplication::setApplicationVersion("1.0.0");

    int viewportWidth;
    int viewportHeight;
    QString sceneFilePath;
    parseOptions(viewportWidth, viewportHeight, sceneFilePath);

    QScopedPointer<QVulkanInstance> vulkanInstance(RenderWindow::createDefaultVulkanInstance());
    if(!vulkanInstance) {
        return 1;
    }

    RenderWindow window;
    window.setWidth(viewportWidth);
    window.setHeight(viewportHeight);
    window.setVulkanInstance(vulkanInstance.get());

    if(sceneFilePath.length() > 0) {
        if(!window.setSourceFile(sceneFilePath)) {
            return 1;
        }
    }
    else {
        if(!window.chooseSourceFile()) {
            return 1;
        }
    }

    window.show();

    return app.exec();
}
