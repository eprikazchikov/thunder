#ifndef ASSETEDITORSMANAGER_H
#define ASSETEDITORSMANAGER_H

#include <QObject>
#include <QMap>
#include <QFileInfo>
#include <QTimer>
#include <QImage>

#include <patterns/asingleton.h>
#include <engine.h>
#include <module.h>

#include "converters/converter.h"

class QFileSystemWatcher;

class ProjectManager;
class CodeManager;

struct Template {
    Template                    () :
        type(MetaType::INVALID) {

    }
    Template                    (const QString &p, uint32_t t = MetaType::INVALID) :
        path(p),
        type(t) {

    }

    QString                     path;
    uint32_t                    type;
};

Q_DECLARE_METATYPE(Template)

class IAssetEditor {
public:
    IAssetEditor                (Engine *engine) :
            m_bModified(false) {
        m_pEngine   = engine;
    }

    virtual void                loadAsset           (IConverterSettings *settings) = 0;

    void                        setModified         (bool value) { m_bModified = value; }
    bool                        isModified          () { return m_bModified; }

protected:
    Engine                     *m_pEngine;

    bool                        m_bModified;

};

class AssetManager : public QObject, public ASingleton<AssetManager> {
    Q_OBJECT
public:
    void                    init                ();

    void                    addEditor           (uint8_t type, IAssetEditor *editor);
    QObject                *openEditor          (const QFileInfo &source);

    int32_t                 resourceType        (const QFileInfo &source);

    void                    removeResource      (const QFileInfo &source);
    void                    renameResource      (const QFileInfo &oldName, const QFileInfo &newName);
    void                    duplicateResource   (const QFileInfo &source);

    bool                    pushToImport        (const QFileInfo &source);
    bool                    import              (const QFileInfo &source, const QFileInfo &target);

    void                    registerConverter   (IConverter *converter);

    static void             findFreeName        (QString &name, const QString &path, const QString &suff = QString());

    static void             saveSettings        (IConverterSettings *settings);

    string                  guidToPath          (const string &guid);
    string                  pathToGuid          (const string &path);

    QImage                  icon                (const QString &path);

public slots:
    void                    reimport            ();

signals:
    void                    ready               ();

    void                    directoryChanged    (const QString &path);
    void                    fileChanged         (const QString &path);

    void                    imported            (const QString &path, uint8_t type);
    void                    importStarted       (int count, const QString &stage);
    void                    importFinished      ();

protected slots:
    void                    onPerform           ();

    void                    onFileChanged       (const QString &path, bool force = false);

    void                    onDirectoryChanged  (const QString &path, bool force = false);

protected:
    friend class ASingleton<AssetManager>;

    typedef QMap<uint8_t, IAssetEditor *>   EditorsMap;
    EditorsMap              m_Editors;

    typedef QMap<QString, uint8_t>          FormatsMap;
    FormatsMap              m_Formats;

    typedef QMap<QString, IConverter *>     ConverterMap;
    ConverterMap            m_Converters;

    VariantMap              m_Guids;
    VariantMap              m_Paths;

    QFileSystemWatcher     *m_pDirWatcher;
    QFileSystemWatcher     *m_pFileWatcher;

    QList<IConverterSettings *>  m_ImportQueue;

    ProjectManager         *m_pProjectManager;
    CodeManager            *m_pCodeManager;

    QTimer                 *m_pTimer;

protected:
    AssetManager            ();
    ~AssetManager           ();

    IConverterSettings     *createSettings      (const QFileInfo &source);

    void                    cleanupBundle       ();
    void                    dumpBundle          ();

    bool                    isOutdated          (IConverterSettings *settings);

    bool                    pushToImport        (IConverterSettings *settings);

    bool                    convert             (IConverterSettings *settings);
};

#endif // ASSETEDITORSMANAGER_H
