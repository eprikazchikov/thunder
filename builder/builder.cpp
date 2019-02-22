#include "builder.h"

#include "log.h"
#include "projectmanager.h"

#include <quazip.h>
#include <quazipfile.h>

#include <QCoreApplication>

Builder::Builder() {
    //connect(AssetManager::instance(), &AssetManager::importFinished, AssetManager::instance(), &AssetManager::rebuildProject);

    connect(AssetManager::instance(), &AssetManager::importFinished, this, &Builder::onImportFinished);

    connect(this, &Builder::packDone, QCoreApplication::instance(), &QCoreApplication::quit);
    connect(this, &Builder::moveDone, this, &Builder::package);
}

void Builder::package(const QString &target) {
    QFileInfo info(target);
    QString dir = info.absolutePath();
#if __APPLE__
    dir     = target + "/Contents/MacOS";
#endif
    dir    += "/base.pak";

    Log(Log::INF) << "Packaging Assets to:" << qPrintable(dir);
    QuaZip zip(dir);
    if(!zip.open(QuaZip::mdCreate)) {
        Log(Log::ERR) << "Can't open package";
        return;
    }
    QuaZipFile outZipFile(&zip);

    QDirIterator it(ProjectManager::instance()->importPath(), QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while(it.hasNext()) {
        QString path = it.next();
        QFileInfo info(path);
        if(info.isFile()) {
            QFile inFile(info.absoluteFilePath());

            string origin   = AssetManager::instance()->guidToPath(info.fileName().toStdString());
            Log(Log::INF) << "\tCoping:" << origin.c_str();

            if(!inFile.open(QIODevice::ReadOnly)) {
                zip.close();
                Log(Log::ERR) << "Can't open input file";
                return;
            }

            if(!outZipFile.open(QIODevice::WriteOnly, QuaZipNewInfo(info.fileName(), info.absoluteFilePath()))) {
                inFile.close();
                zip.close();
                Log(Log::ERR) << "Can't open output file";
                return;
            }
            outZipFile.write(inFile.readAll());

            outZipFile.close();
            inFile.close();
        }
    }
    Log(Log::INF) << "Packaging Done";
    emit packDone();
}

bool copyRecursively(QString sourceFolder, QString destFolder) {
    bool success = false;
    QDir sourceDir(sourceFolder);

    if(!sourceDir.exists()) {
        return false;
    }

    QDir destDir(destFolder);
    if(!destDir.exists()) {
        destDir.mkdir(destFolder);
    }

    QStringList files = sourceDir.entryList(QDir::Files);
    for(int i = 0; i< files.count(); i++) {
        QString srcName     = sourceFolder + QDir::separator() + files[i];
        QString destName    = destFolder   + QDir::separator() + files[i];
        success = QFile::copy(srcName, destName);
        if(!success)
            return false;
    }

    files.clear();
    files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for(int i = 0; i< files.count(); i++) {
        QString srcName     = sourceFolder + QDir::separator() + files[i];
        QString destName    = destFolder   + QDir::separator() + files[i];
        success = copyRecursively(srcName, destName);
        if(!success) {
            return false;
        }
    }
    return true;
}

void Builder::onImportFinished() {
    QString path = AssetManager::instance()->artifact();
    QFileInfo info(path);
    QFileInfo target(ProjectManager::instance()->targetPath() + "/" + info.fileName());

    if((target.isDir() && QDir(target.absoluteFilePath()).removeRecursively()) || QFile::remove(target.absoluteFilePath())) {
        Log(Log::INF) << "Previous build removed.";
    }

    if((info.isDir() && copyRecursively(path, target.absoluteFilePath())) || QFile::copy(path, target.absoluteFilePath())) {
        Log(Log::INF) << "New build copied to:" << qPrintable(target.absoluteFilePath());
        emit moveDone(target.absoluteFilePath());
        return;
    }

    QCoreApplication::exit(0);
}
