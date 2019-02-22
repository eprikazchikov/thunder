#ifndef ATLAS_H
#define ATLAS_H

#include "engine.h"
#include "texture.h"

class ENGINE_EXPORT Atlas : public Object {
    A_REGISTER(Atlas, Object, Resources)

public:
    Atlas                       ();

    ~Atlas                      ();

    uint32_t                    addElement          (const Texture *texture);

    Vector2Vector               shape               (uint32_t index) const;

    Vector4                     uv                  (uint32_t index) const;

    Texture                    *texture             () const;

    void                        clear               ();

    void                        pack                (uint8_t padding);

protected:
    Vector4Vector               m_Elements;

    Texture                    *m_pTexture;

    Texture::Textures           m_Sources;
};

#endif // ATLAS_H
