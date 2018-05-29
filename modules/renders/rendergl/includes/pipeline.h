#ifndef APIPELINE
#define APIPELINE

#include <cstdint>
#include <stack>

#include <amath.h>
#include <controller.h>

#include "resources/atexturegl.h"
#include "resources/amaterialgl.h"
#include "resources/ameshgl.h"

class Engine;
class BaseController;

class ACameraGL;
class ASpriteGL;

class ABlurGL;

class AAmbientOcclusionGL;

class APostProcessor;

class ICommandBuffer;

class RenderTexture;

class APipeline {
public:
    APipeline                   (Engine *engine);

    virtual ~APipeline          ();

    virtual void                draw                (Scene &scene, uint32_t resource);

    virtual RenderTexture      *postProcess         (RenderTexture &source);

    virtual void                resize              (uint32_t width, uint32_t height);

    void                        drawComponents      (Object &object, uint8_t layer);

    Vector2                     screen              () const { return m_Screen; }
    Vector3                     world               () const { return m_World; }

    void                        cameraReset         ();
    Camera                     *activeCamera        ();

    Scene                      *scene               () const { return m_pScene; }

    ICommandBuffer             *buffer              () const { return m_Buffer; }

    void                        overrideController  (IController *controller) { m_pController = controller; }

protected:
    void                        updateShadows       (Object &object);

    void                        analizeScene        (Object &object);

    void                        deferredShading     (Scene &scene, uint32_t resource);

protected:
    typedef map<string, RenderTexture *> TargetMap;

    ICommandBuffer             *m_Buffer;

    IController                *m_pController;

    Engine                     *m_pEngine;

    Scene                      *m_pScene;

    TargetMap                   m_Targets;

    Vector2                     m_Screen;

    Vector3                     m_World;

    list<APostProcessor *>      m_PostEffects;

    Mesh                       *m_pPlane;
    MaterialInstance           *m_pSprite;
};

#endif // APIPELINE
