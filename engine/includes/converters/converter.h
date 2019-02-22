#ifndef BASECONVERTERSETTINGS_H
#define BASECONVERTERSETTINGS_H

#include <QObject>

#include <engine.h>

class ENGINE_EXPORT IConverterSettings {
public:
    IConverterSettings              ();
    virtual ~IConverterSettings     () {}

    virtual uint32_t        type                    () const;
    virtual void            setType                 (uint32_t type);

    virtual bool            isValid                 () const;
    virtual void            setValid                (bool valid);

    virtual const char     *source                  () const;
    virtual void            setSource               (const char *source);

    virtual const char     *destination             () const;
    virtual void            setDestination          (const char *destination);

    virtual const char     *absoluteDestination     () const;
    virtual void            setAbsoluteDestination  (const char *destination);

    uint32_t                crc                     () const;
    void                    setCRC                  (uint32_t crc);

    virtual void            loadProperties          (const QVariantMap &) {}

    uint32_t                subItemsCount           () const;
    const char             *subItem                 (uint32_t index) const;

    void                    addSubItem              (const char *item);

protected:
    bool                    mValid;

    uint32_t                mType;

    uint32_t                mCRC;

    string                  mDestination;
    string                  mAbsoluteDestination;
    string                  mSource;

    vector<string>          mSubItems;
};

class ENGINE_EXPORT IConverter : public QObject {
public:
    enum ContentTypes {
        ContentInvalid              = MetaType::USERTYPE,
        ContentText,
        ContentTexture,
        ContentMaterial,
        ContentMesh,
        ContentAtlas,
        ContentFont,
        ContentAnimation,
        ContentEffect,
        ContentSound,
        ContentCode,
        ContentMap,
        ContentPipeline,
        ContentPrefab,
        ContentLast
    };
public:
    virtual QStringList             suffixes        () const = 0;
    virtual uint32_t                contentType     () const = 0;
    virtual uint32_t                type            () const = 0;
    virtual uint8_t                 convertFile     (IConverterSettings *) = 0;

    virtual IConverterSettings     *createSettings  () const;
};

#endif // BASECONVERTERSETTINGS_H
