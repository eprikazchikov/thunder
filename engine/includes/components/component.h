#ifndef COMPONENT_H
#define COMPONENT_H

#include <engine.h>

class Actor;
class ICommandBuffer;

class ENGINE_EXPORT Component : public Object {
    A_REGISTER(Component, Object, General)

    A_PROPERTIES (
        A_PROPERTY(bool, Enable, Component::isEnable, Component::setEnable)
    )
    A_METHODS(
        A_METHOD(Actor *, Component::actor)
    )

public:
    Component                   ();

    virtual void                draw                    (ICommandBuffer &buffer, int8_t layer);

    Actor                      *actor                   () const;

    bool                        isEnable                () const;

    void                        setEnable               (bool enable);

    bool                        isStarted               () const;

    void                        setStarted              (bool started);

protected:
    bool                        isSerializable          () const;

protected:
    bool                        m_Enable;

    bool                        m_Started;

};

#endif // COMPONENT_H
