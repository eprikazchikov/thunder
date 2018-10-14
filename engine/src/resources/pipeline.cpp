#include "pipeline.h"

#include "controller.h"

#include "components/actor.h"
#include "components/transform.h"
#include "components/scene.h"
#include "components/camera.h"
#include "components/directlight.h"
#include "components/basemesh.h"

#include "resources/mesh.h"
#include "resources/rendertexture.h"

#include "analytics/profiler.h"
#include "log.h"

#include "commandbuffer.h"

#define SM_RESOLUTION_DEFAULT 1024
#define SM_RESOLUTION 2048

#define MAX_LODS 4

#define G_NORMALS   "normalsMap"
#define G_DIFFUSE   "diffuseMap"
#define G_PARAMS    "paramsMap"
#define G_EMISSIVE  "emissiveMap"

#define SELECT_MAP  "selectMap"
#define DEPTH_MAP   "depthMap"
#define SHADOW_MAP  "shadowMap"

Pipeline::Pipeline() :
        m_Buffer(nullptr),
        m_Screen(Vector2(64, 64)),
        m_pSprite(nullptr) {

    m_Buffer    = Engine::objectCreate<ICommandBuffer>();

    Material *mtl   = Engine::loadResource<Material>(".embedded/DefaultSprite.mtl");
    if(mtl) {
        m_pSprite   = mtl->createInstance();
    }
    m_pPlane    = Engine::loadResource<Mesh>(".embedded/plane.fbx");

    RenderTexture *select   = Engine::objectCreate<RenderTexture>();
    select->setTarget(Texture::RGBA8);
    select->apply();
    m_Targets[SELECT_MAP]   = select;
    m_Buffer->setGlobalTexture(SELECT_MAP,  select);

    RenderTexture *depth    = Engine::objectCreate<RenderTexture>();
    depth->setDepth(24);
    depth->apply();
    m_Targets[DEPTH_MAP]    = depth;
    m_Buffer->setGlobalTexture(DEPTH_MAP,   depth);

    RenderTexture *shadow   = Engine::objectCreate<RenderTexture>();
    shadow->setDepth(24);
    depth->apply();
    m_Targets[SHADOW_MAP]   = shadow;
    m_Buffer->setGlobalTexture(SHADOW_MAP,  shadow);

    RenderTexture *normals  = Engine::objectCreate<RenderTexture>();
    normals->setTarget(Texture::RGB10A2);
    normals->apply();
    m_Targets[G_NORMALS]    = normals;
    m_Buffer->setGlobalTexture(G_NORMALS,   normals);

    RenderTexture *diffuse  = Engine::objectCreate<RenderTexture>();
    diffuse->setTarget(Texture::RGBA8);
    diffuse->apply();
    m_Targets[G_DIFFUSE]    = diffuse;
    m_Buffer->setGlobalTexture(G_DIFFUSE,   diffuse);

    RenderTexture *params   = Engine::objectCreate<RenderTexture>();
    params->setTarget(Texture::RGBA8);
    params->apply();
    m_Targets[G_PARAMS]     = params;
    m_Buffer->setGlobalTexture(G_PARAMS,    params);

    RenderTexture *emissive = Engine::objectCreate<RenderTexture>();
    emissive->setTarget(Texture::R11G11B10Float);
    emissive->apply();
    m_Targets[G_EMISSIVE]   = emissive;
    m_Buffer->setGlobalTexture(G_EMISSIVE,  emissive);

    shadow->resize(SM_RESOLUTION, SM_RESOLUTION);
    shadow->setFixed(true);

  //m_pBlur     = new ABlurGL();
  //m_pAO       = new AAmbientOcclusionGL();

  //m_PostEffects.push_back(new AAntiAliasingGL());
  //m_PostEffects.push_back(new ABloomGL());
}

Pipeline::~Pipeline() {
    for(auto it : m_Targets) {
        it.second->deleteLater();
    }
    m_Targets.clear();
}

void Pipeline::draw(Scene &scene, Camera &camera, uint32_t resource) {
    ObjectList filter   = filterComponents(camera.frustumCorners(camera.nearPlane(), camera.farPlane()));

    // Light prepass
    m_Buffer->setGlobalValue("light.ambient", scene.ambient());

    m_Buffer->setRenderTarget(TargetBuffer(), m_Targets[SHADOW_MAP]);
    m_Buffer->clearRenderTarget();

    updateShadows(camera, scene);

    m_Buffer->setViewport(0, 0, m_Screen.x, m_Screen.y);

    // Retrive object id
    m_Buffer->setRenderTarget({m_Targets[SELECT_MAP]}, m_Targets[DEPTH_MAP]);
    m_Buffer->clearRenderTarget(true, Vector4(0.0));

    cameraReset(camera);
    drawComponents(ICommandBuffer::RAYCAST, filter);

    // Fill G buffer pass
    m_Buffer->setRenderTarget({m_Targets[G_NORMALS], m_Targets[G_DIFFUSE], m_Targets[G_PARAMS], m_Targets[G_EMISSIVE]}, m_Targets[DEPTH_MAP]);
    m_Buffer->clearRenderTarget(true, camera.color(), false);

    cameraReset(camera);
    // Draw Opaque pass
    drawComponents(ICommandBuffer::DEFAULT, filter);

    /// \todo Screen Space Ambient Occlusion effect should be defined here

    m_Buffer->setRenderTarget({m_Targets[G_EMISSIVE]}, m_Targets[DEPTH_MAP]);
    // Light pass
    drawComponents(ICommandBuffer::LIGHT, filter);

    cameraReset(camera);
    // Draw Transparent pass
    drawComponents(ICommandBuffer::TRANSLUCENT, filter);

    m_Buffer->setRenderTarget(resource);
    m_Buffer->setScreenProjection();

    m_pSprite->setTexture("texture0", postProcess(*m_Targets[G_EMISSIVE]));
    m_Buffer->drawMesh(Matrix4(), m_pPlane, 0, ICommandBuffer::UI, m_pSprite);
}

void Pipeline::cameraReset(Camera &camera) {
    Matrix4 v, p;
    camera.matrices(v, p);
    camera.setRatio(m_Screen.x / m_Screen.y);
    m_Buffer->setGlobalValue("camera.position", Vector4(camera.actor().transform()->worldPosition(), camera.nearPlane()));
    m_Buffer->setGlobalValue("camera.target", Vector4(Vector3(), camera.farPlane()));
    m_Buffer->setGlobalValue("camera.screen", Vector4(1.0f / m_Screen.x, 1.0f / m_Screen.y, m_Screen.x, m_Screen.y));
    m_Buffer->setGlobalValue("camera.mvpi", (p * v).inverse());
    m_Buffer->setGlobalValue("light.map", Vector4(1.0f / SM_RESOLUTION, 1.0f / SM_RESOLUTION, SM_RESOLUTION, SM_RESOLUTION));
    m_Buffer->setViewProjection(v, p);
}

RenderTexture *Pipeline::target(const string &target) const {
    auto it = m_Targets.find(target);
    if(it != m_Targets.end()) {
        return it->second;
    }
    return nullptr;
}

Mesh *Pipeline::plane() const {
    return m_pPlane;
}

MaterialInstance *Pipeline::sprite() const {
    return m_pSprite;
}

void Pipeline::resize(uint32_t width, uint32_t height) {
    m_Screen    = Vector2(width, height);

    for(auto &it : m_Targets) {
        it.second->resize(width, height);
    }
    //for(auto &it : m_PostEffects) {
    //    //it->resize(width, height);
    //}
}

void Pipeline::combineComponents(Object &object, bool first) {
    if(first) {
        m_Components.clear();
    }
    for(auto &it : object.getChildren()) {
        Object *child   = it;
        BaseMesh *mesh  = dynamic_cast<BaseMesh *>(child);
        if(mesh) {
            if(mesh->isEnable()) {
                m_Components.push_back(mesh);
            }
        } else {
            DirectLight *light  = dynamic_cast<DirectLight *>(child);
            if(light) {
                if(light->isEnable()) {
                    m_Components.push_back(light);
                }
            } else {
                Actor *actor    = dynamic_cast<Actor *>(child);
                if(actor) {
                    if(!actor->isEnable()) {
                        continue;
                    }
                }
                combineComponents(*child);
            }
        }
    }
}

Object::ObjectList Pipeline::filterComponents(const array<Vector3, 8> &frustum) {
    return frustumCulling(m_Components, frustum);
}

void Pipeline::drawComponents(uint32_t layer, ObjectList &list) {
    for(auto it : list) {
        static_cast<Component *>(it)->draw(*m_Buffer, layer);
    }
}

void Pipeline::updateShadows(Camera &camera, Object &object) {
    for(auto &it : object.getChildren()) {
        DirectLight *light = dynamic_cast<DirectLight *>(it);
        if(light) {
            directUpdate(camera, light);
        } else {
            updateShadows(camera, *it);
        }
    }
}

void Pipeline::directUpdate(Camera &camera, DirectLight *light) {
    Vector4 distance;

    Matrix4 p   = camera.projectionMatrix();
    {
        float split     = 0.95f;
        float nearPlane = camera.nearPlane();
        float farPlane  = camera.farPlane();
        for(int i = 0; i < MAX_LODS; i++) {
            float f = (i + 1) / (float)MAX_LODS;
            float l = nearPlane * powf(farPlane / nearPlane, f);
            float u = nearPlane + (farPlane - nearPlane) * f;
            float v = MIX(u, l, split);
            distance[i]     = v;
            Vector4 depth   = p * Vector4(0.0f, 0.0f, -v * 2.0f - 1.0f, 1.0f);
            light->normalizedDistance()[i] = depth.z / depth.w;
        }
    }

    float nearPlane = camera.nearPlane();
    Matrix4 view    = Matrix4(light->actor().transform()->rotation().toMatrix()).inverse();
    for(uint32_t lod = 0; lod < MAX_LODS; lod++) {
        float dist  = distance[lod];
        const array<Vector3, 8> &points = camera.frustumCorners(nearPlane, dist);
        nearPlane   = dist;

        AABBox box;
        box.setBox(&(points.at(0)), 8);

        Vector3 min, max;
        box.box(min, max);

        Vector3 rot[8]  = {
            view * Vector3(min.x, min.y, min.z),
            view * Vector3(min.x, min.y, max.z),
            view * Vector3(max.x, min.y, max.z),
            view * Vector3(max.x, min.y, min.z),

            view * Vector3(min.x, max.y, min.z),
            view * Vector3(min.x, max.y, max.z),
            view * Vector3(max.x, max.y, max.z),
            view * Vector3(max.x, max.y, min.z)
        };
        box.setBox(rot, 8);
        box.box(min, max);

        Matrix4 crop    = Matrix4::ortho(min.x, max.x,
                                         min.y, max.y,
                                         -100, 100);

        m_Buffer->setViewProjection(view, crop);

        light->matrix()[lod]    = Matrix4(Vector3(0.5f), Quaternion(), Vector3(0.5f)) * crop * view;
        // Draw in the depth buffer from position of the light source
        uint32_t x  = (lod % 2) * SM_RESOLUTION_DEFAULT;
        uint32_t y  = (lod / 2) * SM_RESOLUTION_DEFAULT;
        //float ratio = camera->ratio();
        uint32_t w  = SM_RESOLUTION_DEFAULT;// / ((ratio < 1.0f) ? ratio : 1.0f);
        uint32_t h  = SM_RESOLUTION_DEFAULT;// / ((ratio > 1.0f) ? ratio : 1.0f);
        m_Buffer->setViewport(x, y, w, h);

        light->tiles()[lod] = Vector4((float)x / SM_RESOLUTION,
                                      (float)y / SM_RESOLUTION,
                                      (float)w / SM_RESOLUTION,
                                      (float)h / SM_RESOLUTION);

        drawComponents(ICommandBuffer::SHADOWCAST, m_Components);
    }
}

RenderTexture *Pipeline::postProcess(RenderTexture &source) {
    RenderTexture *result   = &source;
    //for(auto it : m_PostEffects) {
    //    result  = it->draw(*result, *m_Buffer);
    //}
    return result;
}

inline bool intersect(Plane pl[6], Vector3 points[8]) {
    for(int i = 0; i < 6; i++) {
        if(pl[i].sqrDistance(points[0]) > 0) {
            continue;
        }
        if(pl[i].sqrDistance(points[1]) > 0) {
            continue;
        }
        if(pl[i].sqrDistance(points[2]) > 0) {
            continue;
        }
        if(pl[i].sqrDistance(points[3]) > 0) {
            continue;
        }
        if(pl[i].sqrDistance(points[4]) > 0) {
            continue;
        }
        if(pl[i].sqrDistance(points[5]) > 0) {
            continue;
        }
        if(pl[i].sqrDistance(points[6]) > 0) {
            continue;
        }
        if(pl[i].sqrDistance(points[7]) > 0) {
            continue;
        }
        return false;
    }
    return true;
}

Object::ObjectList Pipeline::frustumCulling(ObjectList &in, const array<Vector3, 8> &frustum) {
    Plane pl[6];
    pl[0]   = Plane(frustum[1], frustum[0], frustum[4]); // top
    pl[1]   = Plane(frustum[7], frustum[3], frustum[2]); // bottom
    pl[2]   = Plane(frustum[3], frustum[7], frustum[0]); // left
    pl[3]   = Plane(frustum[2], frustum[1], frustum[6]); // right
    pl[4]   = Plane(frustum[0], frustum[1], frustum[3]); // near
    pl[5]   = Plane(frustum[5], frustum[4], frustum[6]); // far

    Object::ObjectList result;
    for(auto it : in) {
        BaseMesh *mesh  = dynamic_cast<BaseMesh *>(it);
        if(mesh && mesh->mesh()) {
            Matrix4 &transform   = mesh->actor().transform()->worldTransform();
            Vector3 min, max;
            mesh->mesh()->bound().box(min, max);
            Matrix3 r   = transform.rotation();
            Vector3 t(transform[12], transform[13], transform[14]);

            Vector3 p[8];
            p[0]    = r * Vector3(min.x, min.y, min.z) + t;
            p[1]    = r * Vector3(min.x, min.y, max.z) + t;
            p[2]    = r * Vector3(max.x, min.y, max.z) + t;
            p[3]    = r * Vector3(max.x, min.y, min.z) + t;
            p[4]    = r * Vector3(min.x, max.y, min.z) + t;
            p[5]    = r * Vector3(min.x, max.y, max.z) + t;
            p[6]    = r * Vector3(max.x, max.y, max.z) + t;
            p[7]    = r * Vector3(max.x, max.y, min.z) + t;

            if(intersect(pl, p)) {
                result.push_back(it);
            }
        } else {
            result.push_back(it);
        }
    }
    return result;
}

