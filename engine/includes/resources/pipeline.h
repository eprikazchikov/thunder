#ifndef APIPELINE
#define APIPELINE

#include <cstdint>
#include <stack>

#include <amath.h>

#include "engine.h"

class ICommandBuffer;

class Component;
class DirectLight;
class Scene;
class Camera;

class Mesh;
class MaterialInstance;
class RenderTexture;
class PostProcessor;

class NEXT_LIBRARY_EXPORT Pipeline : public Object {
    A_REGISTER(Pipeline, Object, Resources)

public:
    Pipeline                    ();

    virtual ~Pipeline           ();

    virtual void                draw                (Scene &scene, Camera &camera, uint32_t resource);

    virtual void                resize              (uint32_t width, uint32_t height);

    void                        cameraReset         (Camera &camera);

    RenderTexture              *target              (const string &target) const;

    Mesh                       *plane               () const;

    MaterialInstance           *sprite              () const;

    void                        combineComponents   (Object &object, bool first = false);

protected:
    ObjectList                  filterComponents    (const array<Vector3, 8> &frustum);

    void                        drawComponents      (uint32_t layer, ObjectList &list);

    void                        updateShadows       (Camera &camera, Object &object);

    void                        directUpdate        (Camera &camera, DirectLight *light);

    RenderTexture              *postProcess         (RenderTexture &source);

    ObjectList                  frustumCulling      (ObjectList &in, const array<Vector3, 8> &frustum);

protected:
    typedef map<string, RenderTexture *> TargetMap;

    ICommandBuffer             *m_Buffer;

    ObjectList                  m_Components;

    TargetMap                   m_Targets;

    Vector2                     m_Screen;

    list<PostProcessor *>       m_PostEffects;

    Mesh                       *m_pPlane;
    MaterialInstance           *m_pSprite;
};

#endif // APIPELINE
