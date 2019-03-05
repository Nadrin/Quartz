/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTextStream>

#include "importer.h"
#include "exporter.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("scene2qml");
    QCoreApplication::setApplicationVersion("1.0.2");

    QCommandLineParser parser;
    parser.setApplicationDescription("3D scene file to Quartz QML converter.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("source", "Source scene file path.");
    parser.addPositionalArgument("output", "Output QML file path.");

    QCommandLineOption prefixOption("p", "Prefix to be added to every asset source path.", "prefix");
    parser.addOption(prefixOption);
    QCommandLineOption meshDirectoryOption("m", "Custom mesh directory (relative to QML file location).", "path");
    parser.addOption(meshDirectoryOption);
    QCommandLineOption textureDirectoryOption("t", "Custom texture directory (relative to QML file location).", "path");
    parser.addOption(textureDirectoryOption);
    QCommandLineOption srgbOption("srgb", "Assume color properties to be in sRGB colorspace.");
    parser.addOption(srgbOption);

    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if(args.size() != 2) {
        parser.showHelp(2);
    }

    const QString sourcePath = args[0];
    const QString targetPath = args[1];
    const QString sourceBasePath = QFileInfo(sourcePath).path();
    const QString targetBaseName = QFileInfo(targetPath).baseName();

    QTextStream(stdout) << QFileInfo(targetPath).absoluteFilePath() << " ...\n";

    Importer importer;
    if(!importer.importScene(sourcePath)) {
        return 1;
    }

    Exporter exporter(importer.scene());

    if(parser.isSet(prefixOption)) {
        exporter.setPrefix(parser.value(prefixOption));
    }

    if(parser.isSet(meshDirectoryOption)) {
        exporter.setMeshDirectory(parser.value(meshDirectoryOption));
    }
    else {
        exporter.setMeshDirectory(QString("%1_meshes").arg(targetBaseName));
    }

    if(parser.isSet(textureDirectoryOption)) {
        exporter.setTexturesDirectory(parser.value(textureDirectoryOption));
    }
    else {
        exporter.setTexturesDirectory(QString("%1_textures").arg(targetBaseName));
    }

    if(parser.isSet(srgbOption)) {
        exporter.setColorspace(Colorspace::sRGB);
    }

    if(!exporter.exportQml(targetPath, QFileInfo(sourcePath).fileName())) {
        return 1;
    }

    bool status = true;
    status &= exporter.exportMeshes();
    status &= exporter.exportTextures(sourceBasePath);

    QTextStream(stdout) << "  "
                        << exporter.numExportedEntities() << " entities, "
                        << exporter.numExportedMeshes()   << " mesh(es), "
                        << exporter.numExportedTextures() << " texture(s)\n";

    return status ? 0 : 1;
}
