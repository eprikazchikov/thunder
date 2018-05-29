#ifndef TEXTUREGL_H
#define TEXTUREGL_H

#include <deque>

#include <resources/texture.h>

#include "agl.h"

class ATextureGL : public Texture {
    A_OVERRIDE(ATextureGL, Texture, Resources)
public:
    ATextureGL                  ();

    void                        apply                       ();

    void                       *nativeHandle                () const { return (void *)m_ID; }

private:
    void                        clear                       ();

    bool                        uploadTexture2D             (const Sides &sides, uint32_t imageIndex = 0, uint32_t target = GL_TEXTURE_2D, uint32_t format = GL_RGBA, bool update = false);
    bool                        uploadTextureCubemap        (const Sides &sides, uint32_t format);

    inline bool                 isDwordAligned              ();
    inline uint32_t             dwordAlignedLineSize        (uint32_t width, uint32_t bpp);

    uint32_t                    m_ID;
};

#endif // TEXTUREGL_H
