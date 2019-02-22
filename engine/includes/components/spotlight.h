#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "baselight.h"

class Mesh;
class MaterialInstance;

class ENGINE_EXPORT SpotLight : public BaseLight {
    A_REGISTER(SpotLight, BaseLight, Components)

    A_PROPERTIES(
        A_PROPERTY(float,   Attenuation_Radius,   SpotLight::radius, SpotLight::setRadius),
        A_PROPERTY(float,   Outer_Angle,          SpotLight::angle, SpotLight::setAngle)
    )

public:
    SpotLight                   ();

    void                        draw                    (ICommandBuffer &buffer, int8_t layer);

    float                       radius                  () const;
    void                        setRadius               (float value);

    float                       angle                   () const;
    void                        setAngle                (float value);

protected:
    Vector3                     m_Position;

    Vector3                     m_Direction;


    float                       m_Angle;

};

#endif // SPOTLIGHT_H
