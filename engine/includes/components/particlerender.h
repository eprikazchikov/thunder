#ifndef PARTICLERENDER_H
#define PARTICLERENDER_H

#include "nativebehaviour.h"

class ParticleRenderPrivate;
class ParticleEffect;

class ENGINE_EXPORT ParticleRender : public NativeBehaviour {
    A_REGISTER(ParticleRender, NativeBehaviour, Components)

    A_PROPERTIES(
        A_PROPERTY(ParticleEffect *, Effect, ParticleRender::effect, ParticleRender::setEffect)
    )
    A_NOMETHODS()

public:
    ParticleRender();

    void update();

    void draw(ICommandBuffer &buffer, int8_t layer);

    ParticleEffect *effect() const;

    void setEffect(ParticleEffect *effect);

private:
    ParticleRenderPrivate *m_effect;

    void loadUserData(const VariantMap &data);
    VariantMap saveUserData() const;
};

#endif // PARTICLERENDER_H
