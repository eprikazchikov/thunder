#ifndef ARENDERTEXTUREGL_H
#define ARENDERTEXTUREGL_H

#include <resources/rendertexture.h>

#include "agl.h"

class ARenderTextureGL : public RenderTexture {
    A_OVERRIDE(ARenderTextureGL, RenderTexture, Resources)

public:
    void                        apply                       ();

    void                        clear                       ();

    uint32_t                    buffer                      () const { return m_Buffer; }

    void                       *nativeHandle                () const { return (void *)m_ID; }

private:
    uint32_t                    m_Buffer;

    uint32_t                    m_ID;

};

#endif // ARENDERTEXTUREGL_H
