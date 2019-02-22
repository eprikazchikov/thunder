#ifndef DIRECTLIGHT_H
#define DIRECTLIGHT_H

#include "baselight.h"

class Mesh;
class MaterialInstance;

class ENGINE_EXPORT DirectLight : public BaseLight {
    A_REGISTER(DirectLight, BaseLight, Components)

    A_NOPROPERTIES()

public:
    DirectLight                 ();

    ~DirectLight                ();

    void                        draw                    (ICommandBuffer &buffer, int8_t layer);

    Vector4                    &normalizedDistance      ();

    Vector4                    *tiles                   ();

    Matrix4                    *matrix                  ();

protected:
    Matrix4                    *m_pMatrix;
    Vector4                    *m_pTiles;

    Vector4                     m_NormalizedDistance;

    Vector3                     m_Direction;
};

#endif // DIRECTLIGHT_H
