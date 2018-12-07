#ifndef ANIMATIONCONTROLLER_H
#define ANIMATIONCONTROLLER_H

#include "component.h"

class PropertyAnimation;
class AnimationClip;

class NEXT_LIBRARY_EXPORT AnimationController : public Component {
    A_REGISTER(AnimationController, Component, Components)

    A_PROPERTIES (
        A_PROPERTY(AnimationClip*, Clip, AnimationController::clip, AnimationController::setClip)
    )

public:
    AnimationController         ();

    void                        start               ();

    void                        update              ();

    AnimationClip              *clip                () const;

    void                        setClip             (AnimationClip *clip);

    uint32_t                    position            () const;

    void                        setPosition         (uint32_t ms);

    uint32_t                    duration            () const;

    void                        loadUserData        (const VariantMap &data);

    VariantMap                  saveUserData        () const;

protected:
    Object                     *findTarget          (Object *src, const string &path);

    list<PropertyAnimation *>   m_Properties;

    AnimationClip              *m_pClip;

    uint32_t                    m_Time;

};

#endif // ANIMATIONCONTROLLER_H
