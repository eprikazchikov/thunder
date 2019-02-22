#ifndef RENDERTEXTURE_H
#define RENDERTEXTURE_H

#include "texture.h"

class ENGINE_EXPORT RenderTexture : public Texture {
    A_REGISTER(RenderTexture, Texture, Resources)

public:
    RenderTexture               ();

    ~RenderTexture              ();

    void                        setTarget                   (FormatType format);

    void                        setDepth                    (uint8_t bits);

    void                        setFixed                    (bool fixed);

    void                        resize                      (uint32_t width, uint32_t height);

    virtual void                makeCurrent                 (uint32_t index = 0) const;

protected:
    uint8_t                     m_DepthBits;

    bool                        m_Fixed;

};

#endif // RENDERTEXTURE_H
