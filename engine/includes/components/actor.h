#ifndef ACTOR_H
#define ACTOR_H

#include "engine.h"

class Scene;
class Component;
class Transform;
class Prefab;

class ENGINE_EXPORT Actor : public Object {
    A_REGISTER(Actor, Object, Scene)

    A_PROPERTIES(
        A_PROPERTY(bool, Enable, Actor::isEnable, Actor::setEnable)
    )
    A_METHODS(
        A_METHOD(Transform *, Actor::transform),
        A_METHOD(Component *, Actor::findComponent),
        A_METHOD(Component *, Actor::createComponent)
    )

public:
    Actor                       ();

    bool                        isEnable                () const;

    uint8_t                     layers                  () const;

    Transform                  *transform               ();

    Scene                      *scene                   () const;

    Component                  *findComponent           (const char *type);

    template<typename T>
    T                          *component               () {
        return static_cast<T *>(findComponent(T::metaClass()->name()));
    }

    void                        setEnable               (const bool enable);

    void                        setLayers               (const uint8_t layers);

    template<typename T>
    T                          *addComponent            () {
        return static_cast<T *>(createComponent(T::metaClass()->name()));
    }

    Component                  *createComponent         (const string type);

    void                        setParent               (Object *parent);

    bool                        isPrefab                () const;

    void                        setPrefab               (Actor *prefab);

    bool                        isSerializable          () const;

protected:
    void                        addChild                (Object *value);

    void                        loadUserData            (const VariantMap &data);

    VariantMap                  saveUserData            () const;

protected:
    uint8_t                     m_Layers;

    bool                        m_Enable;

    Transform                  *m_pTransform;

    Actor                      *m_pPrefab;

    Scene                      *m_pScene;
};

#endif // ACTOR_H
