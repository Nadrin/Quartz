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
#include "version.h"

namespace Config {
static constexpr int DefaultViewportWidth  = 1280;
static constexpr int DefaultViewportHeight = 720;
} // Config

static void parseOptions(int &viewportWidth, int &viewportHeight, QString &sceneFilePath)
{
    const QString description = QString("%1 %2\n%3\n%4")
            .arg(ApplicationDescription)
            .arg(ApplicationVersion)
            .arg(ApplicationCopyright)
            .arg(ApplicationUrl);

    QCommandLineParser parser;
    parser.setApplicationDescription(description);
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
    QApplication::setApplicationName(ApplicationName);
    QApplication::setApplicationVersion(ApplicationVersion);

    int viewportWidth;
    int viewportHeight;
    QString sceneFilePath;
    parseOptions(viewportWidth, viewportHeight, sceneFilePath);

    QTextStream(stdout) << ApplicationDescription << " "
                        << ApplicationVersion << "\n";

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
        if(!window.requestSourceFile()) {
            return 1;
        }
    }

    window.show();

    return app.exec();
}
