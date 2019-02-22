#ifndef IBUILDER_H
#define IBUILDER_H

#include "converter.h"
#include "resources/text.h"

#include <QMap>

class ProjectManager;

typedef QMap<QString, QString>      StringMap;

class ENGINE_EXPORT IBuilder : public IConverter {
    Q_OBJECT
public:
    IBuilder                        ();

    virtual bool                    buildProject        () = 0;

    virtual QString                 builderVersion      () = 0;

    virtual uint32_t                contentType         () const { return IConverter::ContentCode; }
    virtual uint32_t                type                () const { return MetaType::type<Text *>(); }
    virtual uint8_t                 convertFile         (IConverterSettings *);

    void                            copyTemplate        (const QString &src, const QString &dst, StringMap &values);

    QString                         project             () const { return m_Project; }

    void                            rescanSources       (const QString &path);

    bool                            isOutdated          () const { return m_Outdated; }

protected:
    StringMap                       m_Values;

    QString                         m_Suffix;
    QString                         m_Project;

    QStringList                     m_Sources;

    bool                            m_Outdated;
};

#endif // BUILDER_H
