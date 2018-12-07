#include <QApplication>

#include <QDialog>
#include <QSurfaceFormat>

#include <stdio.h>

#include "managers/undomanager/undomanager.h"

#include "editors/scenecomposer/scenecomposer.h"

#include "assetmanager.h"
#include "codemanager.h"
#include "projectmanager.h"
#include <engine.h>

#include <global.h>
#include "qlog.h"

#include <QDesktopServices>
#include <QUrl>

#include <regex>
#include "managers/projectmanager/projectdialog.h"

#include "editors/textureedit/textureedit.h"
#include "editors/materialedit/materialedit.h"
#include "editors/meshedit/meshedit.h"
#include "editors/particleedit/particleedit.h"

int main(int argc, char *argv[]) {
    QSurfaceFormat format;
    format.setVersion(4, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    //format.setRenderableType(QSurfaceFormat::OpenGLES);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName(COMPANY_NAME);
    QCoreApplication::setApplicationName(EDITOR_NAME);
    QCoreApplication::setApplicationVersion(SDK_VERSION);

    QFile qss(":/Style/styles/dark/style.qss");
    if(qss.open(QIODevice::ReadOnly)) {
        qApp->setStyleSheet(qss.readAll());
        qss.close();
    }

    ProjectManager *mgr = ProjectManager::instance();

    QString project;
    if(argc > 1) {
        project = QApplication::arguments().at(1);
    } else {
        project = ProjectDialog::projectPath();
    }
    int result = 0;
    if(!project.isEmpty()) {
        mgr->init(project);

        IFile *file = new IFile();
        file->finit(qPrintable(QApplication::arguments().at(0)));
        file->fsearchPathAdd(qPrintable(mgr->importPath()), true);

        Engine engine(file, argc, argv);
        engine.init();

        AssetManager *asset = AssetManager::instance();
        asset->addEditor(IConverter::ContentTexture, new TextureEdit(&engine));
        asset->addEditor(IConverter::ContentMaterial, new MaterialEdit(&engine));
        asset->addEditor(IConverter::ContentMesh, new MeshEdit(&engine));
        asset->addEditor(IConverter::ContentEffect, new ParticleEdit(&engine));

        SceneComposer w(&engine);
        QApplication::connect(AssetManager::instance(), SIGNAL(importFinished()), &w, SLOT(show()));

        CodeManager::instance()->init();
        asset->init(&engine);
        UndoManager::instance()->init();

        result  = a.exec();
    }
    UndoManager::destroy();
    AssetManager::destroy();
    CodeManager::destroy();
    return result;
}
