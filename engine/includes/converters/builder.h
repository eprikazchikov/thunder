#ifndef IBUILDER_H
#define IBUILDER_H

#include "converter.h"

#include <QMap>

class ProjectManager;

class NEXT_LIBRARY_EXPORT IBuilder : public IConverter {
    Q_OBJECT
public:
    IBuilder ();

    virtual bool buildProject () = 0;

    virtual QString builderVersion () = 0;

    virtual uint32_t contentType () const { return IConverter::ContentCode; }
    virtual uint32_t type () const;
    virtual uint8_t convertFile (IConverterSettings *);

    virtual const QString persistentAsset() const { return ""; }
    virtual const QString persistentUUID () const { return ""; }

    QString project () const { return m_Project; }

    void rescanSources (const QString &path);
    bool isEmpty () const;

    bool isOutdated () const { return m_Outdated; }

protected:
    void copyTemplate (const QString &src, const QString &dst, QStringMap &values);
    void generateLoader (const QString &dst);

    QString formatList(const QStringList &list);

protected:
    QStringMap m_Values;

    QString m_Project;

    QStringList m_Sources;

    bool m_Outdated;
};

#endif // IBUILDER_H
