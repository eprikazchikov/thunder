#include "qbsbuilder.h"

#include <QProcess>
#include <QDir>
#include <QStandardPaths>

#include <log.h>

#include <projectmanager.h>

const QString gFilesList("${filesList}");

const QString gSdkPath("${sdkPath}");

const QString gIncludePaths("${includePaths}");
const QString gLibraryPaths("${libraryPaths}");
const QString gLibraries("${libraries}");

const QString gEditorSuffix("-Editor");

// generate --generator visualstudio2013

QbsBuilder::QbsBuilder() :
        IBuilder() {

    m_Settings << "--settings-dir" << QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/..";

    if(m_pMgr->targetPath().isEmpty()) {
        m_Artifact  = m_Project + "debug/install-root/" + m_pMgr->projectName() + gEditorSuffix + m_Suffix;
    } else {
        m_Artifact  = m_Project + "release/install-root/" + m_pMgr->projectName() + m_Suffix;
    }

    m_pProcess  = new QProcess(this);
    m_pProcess->setWorkingDirectory(m_Project);

    connect( m_pProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()) );
    connect( m_pProcess, SIGNAL(readyReadStandardError()), this, SLOT(readError()) );

    connect( m_pProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SIGNAL(buildFinished(int)) );

#if _WIN32
    m_Profiles << "MSVC2015-x86";
#elif __APPLE__
    m_Profiles << "xcode-macosx-x86_64";
#endif
    m_Profiles << "Android";

    if(!checkProfile(m_Profiles[0])) {
        Log(Log::INF) << "Initializing QBS...";
        builderInit();
    }
}

void QbsBuilder::generateProject(const QStringList &code) {
    StringMap values(m_Values);

    values[gSdkPath]        = m_pMgr->sdkPath();
    values[gIncludePaths]   = formatList(m_IncludePath);
    values[gLibraryPaths]   = formatList(m_LibPath);
    values[gFilesList]      = formatList(code);
    values[gLibraries]      = formatList(m_Libs);

    copyTemplate(m_pMgr->resourcePath() + "/editor/templates/project.qbs", m_Project + m_pMgr->projectName() + ".qbs", values);
}

bool QbsBuilder::buildProject() {
    QStringList args;
    args << "build" << m_Settings;

    QString mode    = "release";
    QString product = m_pMgr->projectName();
    if(m_pMgr->targetPath().isEmpty()) {
        mode        = "debug";
        product    += gEditorSuffix;
    }
    args << "--products" << product << mode << "profile:" + m_Profiles[0] ;

    m_pProcess->start(m_pMgr->qbsPath(), args);
    if(!m_pProcess->waitForStarted()) {
        Log(Log::ERR) << "Failed:" << qPrintable(m_pProcess->errorString()) << qPrintable(m_pMgr->qbsPath());
        return false;
    }

    return true;
}

QString QbsBuilder::builderVersion() {
    QStringList args;
    args << "--version";

    QProcess qbs(this);

    qbs.setWorkingDirectory(m_Project);
    qbs.start(m_pMgr->qbsPath(), args);
    if(qbs.waitForStarted() && qbs.waitForFinished()) {
        return qbs.readAll().simplified();
    }
    return QString();
}

void QbsBuilder::builderInit() {
    {
        QStringList args;
        args << "setup-toolchains" << "--detect" << m_Settings;
        QProcess qbs(this);
        qbs.setWorkingDirectory(m_Project);
        qbs.start(m_pMgr->qbsPath(), args);
        if(qbs.waitForStarted()) {
            qbs.waitForFinished();
            Log(Log::INF) << "Failed:" << qbs.readAll().constData();
        }
    }
    {
        QStringList args;
        args << "setup-android" << m_Settings;
        args << "--sdk-dir" << "D:/Environment/Android/sdk";
        args << "--ndk-dir" << "D:/Environment/Android/sdk/ndk-bundle";
        //D:/Environment/Android/sdk/ndk-bundle/sysroot
        args << "Android";

        QProcess qbs(this);
        qbs.setWorkingDirectory(m_Project);
        qbs.start(m_pMgr->qbsPath(), args);
        if(qbs.waitForStarted()) {
            qbs.waitForFinished();
        }
    }
}

void QbsBuilder::readOutput() {
    QProcess *p = dynamic_cast<QProcess *>( sender() );
    if(p) {
        parseLogs(p->readAllStandardOutput());
    }
}

void QbsBuilder::readError() {
    QProcess *p = dynamic_cast<QProcess *>( sender() );
    if(p) {
        parseLogs(p->readAllStandardError());
    }
}

QString QbsBuilder::formatList(const QStringList &list) {
    bool first  = true;
    QString result;
    foreach(QString it, list) {
        result += QString("%2\n\t\t\t\"%1\"").arg(it).arg((!first) ? "," : "");
        first   = false;
    }
    return result;
}

void QbsBuilder::parseLogs(const QString &log) {
    QStringList list = log.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

    foreach(QString it, list) {
        if(it.contains(" error ")) {
            Log(Log::ERR) << qPrintable(it);
        } else if(it.contains(" warning ")) {
            Log(Log::WRN) << qPrintable(it);
        } else {
            Log(Log::INF) << qPrintable(it);
        }
    }
}

bool QbsBuilder::checkProfile(const QString &profile) {
    QStringList args;
    args << "config" << "--list" << m_Settings;
    QProcess qbs(this);
    qbs.setWorkingDirectory(m_Project);
    qbs.start(m_pMgr->qbsPath(), args);
    if(qbs.waitForStarted() && qbs.waitForFinished()) {
        return qbs.readAll().contains(qPrintable(profile));
    }
    return false;
}
