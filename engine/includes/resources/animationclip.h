#ifndef ANIMATIONCLIP_H
#define ANIMATIONCLIP_H

#include "variantanimation.h"
#include "engine_export.h"

class ENGINE_EXPORT AnimationClip : public Object {
    A_REGISTER(AnimationClip, Object, Resources)

public:
   struct Track {
       string path;

       string property;

       map<int32_t, AnimationCurve> curves;
   };
   typedef list<Track> TrackList;

public:
    void loadUserData(const VariantMap &data);

    static bool compare(const AnimationCurve::KeyFrame &first, const AnimationCurve::KeyFrame &second);

public:
    TrackList m_Tracks;

};

#endif // ANIMATIONCLIP_H
