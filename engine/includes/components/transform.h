#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "component.h"

class ENGINE_EXPORT Transform : public Component {
    A_REGISTER(Transform, Component, Components)

    A_PROPERTIES(
        A_PROPERTY(Vector3, Position, Transform::position, Transform::setPosition),
        A_PROPERTY(Vector3, Rotation, Transform::euler, Transform::setEuler),
        A_PROPERTY(Vector3, Scale, Transform::scale, Transform::setScale)
    )
    A_NOMETHODS()

public:
    Transform                   ();

    Vector3                     position                () const;

    Vector3                     euler                   () const;

    Vector3                     scale                   () const;

    virtual Quaternion          rotation                () const;

    virtual Matrix4            &worldTransform          ();

    virtual Vector3             worldPosition           () const;

    virtual Vector3             worldEuler              () const;

    virtual Quaternion          worldRotation           () const;

    virtual Vector3             worldScale              () const;

    void                        setPosition             (const Vector3 &value);

    void                        setEuler                (const Vector3 &value);

    void                        setScale                (const Vector3 &value);

    virtual void                setRotation             (const Quaternion &value);

protected:
    bool                        m_Dirty;

    Vector3                     m_Position;
    Vector3                     m_Euler;
    Quaternion                  m_Rotation;
    Vector3                     m_Scale;

    Matrix4                     m_Transform;
};

#endif // TRANSFORM_H
