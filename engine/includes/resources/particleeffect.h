#ifndef PARTICLEEFFECT_H
#define PARTICLEEFFECT_H

#include "engine.h"

#include <deque>

class MaterialInstance;
class Mesh;

class ParticleModificator;
typedef deque<ParticleModificator *> ModifiersDeque;

class ENGINE_EXPORT ParticleEffect : public Object {
    A_REGISTER(ParticleEffect, Object, Resources)
public:
    enum ModificatorType {
        MODIFICATOR_LIFETIME    = 1,
        MODIFICATOR_STARTSIZE,
        MODIFICATOR_STARTCOLOR,
        MODIFICATOR_STARTANGLE,
        MODIFICATOR_STARTPOSITION,

        MODIFICATOR_SCALESIZE   = 50,
        MODIFICATOR_SCALECOLOR,
        MODIFICATOR_SCALEANGLE,
        MODIFICATOR_VELOCITY
    };

    struct Emitter {
        bool m_Gpu;

        bool m_Local;

        bool m_Continous;

        float m_Distibution;

        Mesh *m_pMesh;

        MaterialInstance *m_pMaterial;

        ModifiersDeque m_Modifiers;
    };
    typedef deque<Emitter>      EmitterDeque;

    struct ParticleData {
        ParticleData();

        /// Current life of the particle in seconds
        float life;

        /// Current animation frame
        float frame;

        /// Distance to camera
        float distance;

        /// Transformed position of particle
        Vector3 transform;

        /// Current rotation of particle in rads
        Vector3 angle;

        /// Current color and alpha of particle
        Vector4 color;

        /// Current size of particle
        Vector3 size;

        /// Delta to change color and alpha of particle every second
        Vector4 colrate;

        /// Current position of the particle
        Vector3 position;

        /// Delta to change position of particle every second
        Vector3 velocity;

        /// Delta to change rotation of particle every second in degrees
        Vector3 anglerate;

        /// Delta to change size of particle every second
        Vector3 sizerate;
    };

public:
    ParticleEffect();

    ~ParticleEffect();

    void apply();
    void clear();

public:
    uint32_t emittersCount() const;

    Emitter &emitter(uint32_t index);

    void spawnParticle(uint32_t index, ParticleData &data);

    void updateParticle(uint32_t index, ParticleData &data, float dt);

    void loadUserData(const VariantMap &data);

protected:
    EmitterDeque m_Emitters;

};

#endif // PARTICLEEFFECT_H
