#ifndef SCENE_H
#define SCENE_H

#include "engine.h"

class NEXT_LIBRARY_EXPORT Scene : public Object {
    A_REGISTER(Scene, Object, General);

public:
    Scene               ();

    void                start               ();

    void                update              ();

    float               ambient             () const;
    void                setAmbient          (float ambient);

protected:
    void                startComponents     (Object &object);

    void                updateComponents    (Object &object);

    float               m_Ambient;

};

#endif // SCENE_H
