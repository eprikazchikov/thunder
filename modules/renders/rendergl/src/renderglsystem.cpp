#include "renderglsystem.h"

#include "agl.h"

#include <components/camera.h>
#include <components/scene.h>

#include <resources/pipeline.h>

#include <controller.h>

#include <analytics/profiler.h>
#include <log.h>

#include "resources/atexturegl.h"
#include "resources/arendertexturegl.h"

#include "commandbuffergl.h"

RenderGLSystem::RenderGLSystem(Engine *engine) :
        ISystem(engine),
        m_pController(nullptr) {
    PROFILER_MARKER;

    ObjectSystem system;

    ATextureGL::registerClassFactory(&system);
    ARenderTextureGL::registerClassFactory(&system);
    AMaterialGL::registerClassFactory(&system);
    AMeshGL::registerClassFactory(&system);

    CommandBufferGL::registerClassFactory(&system);
}

RenderGLSystem::~RenderGLSystem() {
    PROFILER_MARKER;

    ObjectSystem system;

    ATextureGL::unregisterClassFactory(&system);
    ARenderTextureGL::unregisterClassFactory(&system);
    AMaterialGL::unregisterClassFactory(&system);
    AMeshGL::unregisterClassFactory(&system);

    CommandBufferGL::unregisterClassFactory(&system);
}

/*!
    Initialization of render.
*/
bool RenderGLSystem::init() {
    PROFILER_MARKER;

#ifndef THUNDER_MOBILE
    if(!gladLoadGL()) {
        Log(Log::ERR) << "[ Render::RenderGLSystem ] Failed to initialize OpenGL context";
        return false;
    }
    glEnable        (GL_TEXTURE_CUBE_MAP_SEAMLESS);
#endif

    int32_t targets;
    glGetIntegerv	(GL_MAX_DRAW_BUFFERS, &targets);

    return true;
}

const char *RenderGLSystem::name() const {
    return "RenderGL";
}

/*!
    Main drawing procedure.
*/
void RenderGLSystem::update(Scene &scene, uint32_t resource) {
    PROFILER_MARKER;

    PROFILER_RESET(POLYGONS);
    PROFILER_RESET(DRAWCALLS);

    Camera *camera  = m_pEngine->controller()->activeCamera();
    if(m_pController) {
        camera  = m_pController->activeCamera();
    }

    if(camera) {
        Pipeline *pipe  = camera->pipeline();
        pipe->combineComponents(scene, true);
        pipe->draw(scene, *camera, resource);
    }
}

void RenderGLSystem::overrideController(IController *controller) {
    PROFILER_MARKER;

    m_pController   = controller;
}
