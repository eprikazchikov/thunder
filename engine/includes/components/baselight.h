#ifndef BASELIGHT_H
#define BASELIGHT_H

#include "component.h"

#include <amath.h>

class Mesh;
class MaterialInstance;

class NEXT_LIBRARY_EXPORT BaseLight : public Component {
    A_REGISTER(BaseLight, Component, Components);

    A_PROPERTIES(
        A_PROPERTY(bool,    Cast_shadows,   BaseLight::castShadows, BaseLight::setCastShadows),
        A_PROPERTY(float,   Brightness,     BaseLight::brightness, BaseLight::setBrightness),
        A_PROPERTY(Color,   Color,          BaseLight::color, BaseLight::setColor),
        A_PROPERTY(float,   Bias,           BaseLight::bias, BaseLight::setBias)
    );

public:
    BaseLight                   ();

    ~BaseLight                  ();

    bool                        castShadows             () const;
    void                        setCastShadows          (bool shadows);

    float                       brightness              () const;
    void                        setBrightness           (const float brightness);

    Vector4                     color                   () const;
    void                        setColor                (const Vector4 &color);

    float                       bias                    () const;
    void                        setBias                 (const float bias);

protected:
    float                       m_Shadows;

    float                       m_Brightness;

    float                       m_Bias;

    Vector4                     m_Color;

    Mesh                       *m_pShape;

    MaterialInstance           *m_pMaterialInstance;
};

#endif // BASELIGHT_H
