#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "baselight.h"

class Mesh;
class MaterialInstance;

class ENGINE_EXPORT PointLight : public BaseLight {
    A_REGISTER(PointLight, BaseLight, Components)

    A_PROPERTIES(
        A_PROPERTY(float,   Attenuation_Radius,   PointLight::radius, PointLight::setRadius)
    )

public:
    PointLight                  ();

    void                        draw                    (ICommandBuffer &buffer, int8_t layer);

    float                       radius                  () const;
    void                        setRadius               (float value);

protected:
    Vector3                     m_Position;

};

#endif // POINTLIGHT_H
