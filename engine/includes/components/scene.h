#ifndef SCENE_H
#define SCENE_H

#include "engine.h"

class ENGINE_EXPORT Scene : public Object {
    A_REGISTER(Scene, Object, General)

public:
    Scene               ();

    float               ambient             () const;
    void                setAmbient          (float ambient);

protected:
    float               m_Ambient;

};

#endif // SCENE_H
