#ifndef TEXTURE_H
#define TEXTURE_H

#include <amath.h>

#include "engine.h"

class Node;

class ENGINE_EXPORT Texture : public Object {
    A_REGISTER(Texture, Object, Resources)

public:
    enum TextureType {
        Flat,
        Cubemap
    };

    enum FormatType {
        R8,
        RGB8,
        RGBA8,
        RGB10A2,
        R11G11B10Float,
        Depth
    };

    enum CompressionType {
        Uncompressed,
        DXT1,
        DXT5,
        ETC2
    };

    enum FilteringType {
        None,
        Bilinear,
        Trilinear
    };

    enum WrapType {
        Clamp,
        Repeat,
        Mirrored
    };

    typedef deque<uint8_t *>    Surface;
    typedef deque<Surface>      Sides;

    typedef deque<const Texture *>  Textures;

public:
    Texture                     ();

    virtual ~Texture            ();

    virtual void                apply                       ();
    virtual void                clear                       ();

    virtual void               *nativeHandle                () const;

    virtual void                readPixels                  (int32_t x, int32_t y, uint32_t width, uint32_t height);

    uint32_t                    getPixel                    (int32_t x, int32_t y) const;

    uint32_t                    width                       () const;
    uint32_t                    height                      () const;

    void                        setWidth                    (uint32_t width);
    void                        setHeight                   (uint32_t height);

    Vector2Vector               shape                       () const;
    void                        setShape                    (const Vector2Vector &shape);

    Vector4Vector               pack                        (const Textures &textures, uint8_t padding = 0);

    bool                        isCompressed                () const;
    bool                        isCubemap                   () const;

    void                        loadUserData                (const VariantMap &data);

    void                        addSurface                  (const Surface &surface);

    void                        resize                      (uint32_t width, uint32_t height);

    void                        setFormat                   (FormatType type);

protected:
    uint32_t                    size                        (uint32_t width, uint32_t height) const;
    uint32_t                    sizeDXTc                    (uint32_t width, uint32_t height) const;
    uint32_t                    sizeRGB                     (uint32_t width, uint32_t height) const;

    uint8_t                     components                  () const;

    FormatType                  m_Format;
    CompressionType             m_Compress;
    TextureType                 m_Type;
    FilteringType               m_Filtering;
    WrapType                    m_Wrap;

    uint32_t                    m_Width;
    uint32_t                    m_Height;

    Vector2Vector               m_Shape;

    Sides                       m_Sides;

    Node                       *m_pRoot;
};

#endif // TEXTURE_H
