#ifndef SPRITEMESH
#define SPRITEMESH

#include "basemesh.h"

class ENGINE_EXPORT SpriteMesh : public BaseMesh {
    A_REGISTER(SpriteMesh, BaseMesh, Components)

    A_PROPERTIES(
        A_PROPERTY(MateralArray, Material, SpriteMesh::materials, SpriteMesh::setMaterials),
        A_PROPERTY(Texture*, Texture, SpriteMesh::texture, SpriteMesh::setTexture)
    )
    A_NOMETHODS()

public:
    SpriteMesh                  ();

    Vector2                     center              () const;

    void                        setCenter           (const Vector2 &value);

    Texture                    *texture             () const;

    virtual void                setTexture          (Texture *texture);

    void                        loadUserData        (const VariantMap &data);

    VariantMap                  saveUserData        () const;

protected:
    Vector2                     m_Center;

    Texture                    *m_Texture;

};

#endif // SPRITEMESH

