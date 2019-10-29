#include "handles.h"

#include <commandbuffer.h>
#include <components/camera.h>
#include <components/actor.h>
#include <components/transform.h>

#include <resources/material.h>

#include "handletools.h"

#define SIDES 32
#define ALPHA 0.3f

#define CONTROL_SIZE 90.0f

#define OVERRIDE "uni.texture0"

Vector4 Handles::s_Normal   = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
Vector4 Handles::s_Selected = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
Vector4 Handles::s_xColor   = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
Vector4 Handles::s_yColor   = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
Vector4 Handles::s_zColor   = Vector4(0.0f, 0.0f, 1.0f, 1.0f);

Vector4 Handles::s_Color    = Handles::s_Normal;
Vector4 Handles::s_Second   = Handles::s_Normal;

Vector2 Handles::m_sMouse   = Vector2();
Vector2 Handles::m_sScreen  = Vector2();

const Vector4 grey(0.3f, 0.3f, 0.3f, 0.6f);

uint8_t Handles::s_Axes     = 0;

static ICommandBuffer *s_Buffer = nullptr;

static float length    = 1.0f;
static float sense     = 0.02f;
static float conesize  = length / 5.0f;

Mesh *Handles::s_Cone   = nullptr;
Mesh *Handles::s_Quad   = nullptr;
Mesh *Handles::s_Lines  = nullptr;

MaterialInstance *Handles::s_Gizmo  = nullptr;
MaterialInstance *Handles::s_Sprite = nullptr;

Mesh *Handles::s_Axis       = nullptr;
Mesh *Handles::s_Scale      = nullptr;
Mesh *Handles::s_ScaleXY    = nullptr;
Mesh *Handles::s_ScaleXYZ   = nullptr;
Mesh *Handles::s_Move       = nullptr;
Mesh *Handles::s_MoveXY     = nullptr;
Mesh *Handles::s_Arc        = nullptr;
Mesh *Handles::s_Circle     = nullptr;
Mesh *Handles::s_Box        = nullptr;

enum {
    AXIS,
    SCALE,
    SCALE_XY,
    SCALE_XYZ,
    MOVE,
    MOVE_XY,
    CIRCLE
};

void Handles::init() {
    if(s_Quad == nullptr) {
        s_Quad = Engine::loadResource<Mesh>(".embedded/plane.fbx/Plane001");
    }
    if(s_Sprite == nullptr) {
        Material *m = Engine::loadResource<Material>(".embedded/DefaultSprite.mtl");
        if(m) {
            MaterialInstance *inst  = m->createInstance();
            s_Sprite = inst;
        }
    }

    if(s_Cone == nullptr) {
        s_Cone = Engine::loadResource<Mesh>(".embedded/cone.fbx/Cone001");
    }
    if(s_Gizmo == nullptr) {
        Material *m = Engine::loadResource<Material>(".embedded/gizmo.mtl");
        if(m) {
            s_Gizmo = m->createInstance();
        }
    }

    if(s_Lines == nullptr) {
        s_Lines = Engine::objectCreate<Mesh>("Lines");
        s_Lines->makeDynamic();
    }

    if(s_Axis == nullptr) {
        Mesh::Lod lod;
        lod.vertices    = {Vector3(0.0f), Vector3(0, 5, 0)};
        lod.indices     = {0, 1};

        s_Axis = Engine::objectCreate<Mesh>("Axis");
        s_Axis->setMode(Mesh::MODE_LINES);
        s_Axis->addLod(lod);
    }

    if(s_Scale == nullptr) {
        Mesh::Lod lod;
        lod.vertices    = {Vector3(0, 2, 0), Vector3(1, 1, 0), Vector3(0, 3, 0), Vector3(1.5, 1.5, 0)};
        lod.indices     = {0, 1, 2, 3};

        s_Scale = Engine::objectCreate<Mesh>("Scale");
        s_Scale->setMode(Mesh::MODE_LINES);
        s_Scale->addLod(lod);
    }

    if(s_ScaleXY == nullptr) {
        Mesh::Lod lod;
        lod.vertices    = {Vector3(0, 2, 0), Vector3(1, 1, 0), Vector3(0, 3, 0), Vector3(1.5, 1.5, 0)};
        lod.indices     = {0, 1, 2, 1, 3, 2};

        s_ScaleXY = Engine::objectCreate<Mesh>("ScaleXY");
        s_ScaleXY->setMode(Mesh::MODE_TRIANGLES);
        s_ScaleXY->addLod(lod);
    }

    if(s_ScaleXYZ == nullptr) {
        Mesh::Lod lod;
        lod.vertices    = {Vector3(0, 2, 0), Vector3(0, 0, 0), Vector3(1, 1, 0)};
        lod.indices     = {0, 1, 2};

        s_ScaleXYZ = Engine::objectCreate<Mesh>("ScaleXYZ");
        s_ScaleXYZ->setMode(Mesh::MODE_TRIANGLES);
        s_ScaleXYZ->addLod(lod);
    }

    if(s_Move == nullptr) {
        Mesh::Lod lod;
        lod.vertices    = {Vector3(0, 1, 0), Vector3(2, 1, 0)};
        lod.indices     = {0, 1};

        s_Move = Engine::objectCreate<Mesh>("Move");
        s_Move->setMode(Mesh::MODE_LINES);
        s_Move->addLod(lod);
    }

    if(s_MoveXY == nullptr) {
        Mesh::Lod lod;
        lod.vertices    = {Vector3(0,-1, 0), Vector3(2,-1, 0), Vector3(0, 1, 0), Vector3(2, 1, 0)};
        lod.indices     = {0, 1, 2, 1, 3, 2};

        s_MoveXY = Engine::objectCreate<Mesh>("MoveXY");
        s_MoveXY->setMode(Mesh::MODE_TRIANGLES);
        s_MoveXY->addLod(lod);
    }

    if(s_Arc == nullptr) {
        Mesh::Lod lod;
        lod.vertices    = HandleTools::pointsArc(Quaternion(), 1.0, 0, 180);
        lod.indices.clear();

        s_Arc = Engine::objectCreate<Mesh>("Arc");
        s_Arc->setMode(Mesh::MODE_LINE_STRIP);
        s_Arc->addLod(lod);
    }

    if(s_Circle == nullptr) {
        Mesh::Lod lod;
        lod.vertices    = HandleTools::pointsArc(Quaternion(), 1.0, 0, 360);
        lod.indices.clear();

        s_Circle = Engine::objectCreate<Mesh>("Circle");
        s_Circle->setMode(Mesh::MODE_LINE_STRIP);
        s_Circle->addLod(lod);
    }

    if(s_Box == nullptr) {
        Mesh::Lod lod;

        Vector3 min(-0.5);
        Vector3 max( 0.5);

        lod.vertices = {
            Vector3(min.x, min.y, min.z),
            Vector3(max.x, min.y, min.z),
            Vector3(max.x, min.y, max.z),
            Vector3(min.x, min.y, max.z),

            Vector3(min.x, max.y, min.z),
            Vector3(max.x, max.y, min.z),
            Vector3(max.x, max.y, max.z),
            Vector3(min.x, max.y, max.z)
        };
        lod.indices   = {0, 1, 1, 2, 2, 3, 3, 0,
                         4, 5, 5, 6, 6, 7, 7, 4,
                         0, 4, 1, 5, 2, 6, 3, 7};

        s_Box = Engine::objectCreate<Mesh>("Box");
        s_Box->setMode(Mesh::MODE_LINES);
        s_Box->addLod(lod);
    }
}

void Handles::beginDraw(ICommandBuffer *buffer) {
    if(ICommandBuffer::isInited()) {
        Matrix4 v, p;
        Camera *cam = Camera::current();
        if(cam) {
            cam->matrices(v, p);
        }

        s_Buffer    = buffer;

        HandleTools::setViewProjection(v, p);
        s_Buffer->setColor(s_Normal);
        s_Buffer->setViewProjection(v, p);
        s_Buffer->clearRenderTarget(false, Vector4(), true, 1.0f);
    }
}

void Handles::endDraw() {
    if(ICommandBuffer::isInited()) {
        s_Buffer->setColor(s_Normal);
    }
}

void Handles::drawArrow(const Matrix4 &transform) {
    if(ICommandBuffer::isInited()) {
        s_Buffer->setColor(s_Color);
        s_Buffer->drawMesh(transform, s_Axis, ICommandBuffer::TRANSLUCENT, s_Gizmo);

        Matrix4 m;
        m.translate(Vector3(0, 4.0, 0));
        s_Buffer->setColor(s_Second);
        s_Buffer->drawMesh(transform * m, s_Cone, ICommandBuffer::TRANSLUCENT, s_Gizmo);
    }
}

void Handles::drawLines(const Matrix4 &transform, const Vector3Vector &points, const Mesh::IndexVector &indices) {
    if(ICommandBuffer::isInited()) {
        Mesh::Lod lod;
        lod.vertices    = points;
        lod.indices     = indices;
        {
            s_Lines->setMode(Mesh::MODE_LINES);
            s_Lines->setLod(0, lod);
        }
        if(s_Buffer) {
            s_Buffer->setColor(s_Color);
            s_Buffer->drawMesh(transform, s_Lines, ICommandBuffer::TRANSLUCENT, s_Gizmo);
        }
    }
}

void Handles::drawBox(const Vector3 &center, const Quaternion &rotation, const Vector3 &size) {
    if(ICommandBuffer::isInited()) {
        s_Buffer->setColor(s_Color);

        Matrix4 transform(center, rotation, size);

        s_Buffer->drawMesh(transform, s_Box, ICommandBuffer::TRANSLUCENT, s_Gizmo);
    }
}

void Handles::drawDisk(const Vector3 &center, const Quaternion &rotation, float radius, float start, float angle) {
    if(ICommandBuffer::isInited()) {
        Mesh::Lod lod;
        lod.vertices = HandleTools::pointsArc(rotation, radius, start, angle, true);
        {
            s_Lines->setMode(Mesh::MODE_TRIANGLE_FAN);
            s_Lines->setLod(0, lod);
        }
        if(s_Buffer) {
            Matrix4 transform;
            transform.translate(center);

            s_Buffer->setColor(s_Color);
            s_Buffer->drawMesh(transform, s_Lines, ICommandBuffer::TRANSLUCENT, s_Gizmo);
        }
    }
}

void Handles::drawCircle(const Vector3 &center, const Quaternion &rotation, float radius) {
    if(ICommandBuffer::isInited()) {
        s_Buffer->setColor(s_Color);

        Matrix4 transform(center, rotation, Vector3(radius));

        s_Buffer->drawMesh(transform, s_Circle, ICommandBuffer::TRANSLUCENT, s_Gizmo);
    }
}

void Handles::drawSphere(const Vector3 &center, const Quaternion &rotation, float radius) {
    if(ICommandBuffer::isInited()) {
        s_Buffer->setColor(s_Color);

        Matrix4 transform(center, rotation, Vector3(radius));

        s_Buffer->drawMesh(transform, s_Circle, ICommandBuffer::TRANSLUCENT, s_Gizmo);
        s_Buffer->drawMesh(transform * Matrix4(Quaternion(Vector3(1, 0, 0), 90).toMatrix()), s_Circle, ICommandBuffer::TRANSLUCENT, s_Gizmo);
        s_Buffer->drawMesh(transform * Matrix4(Quaternion(Vector3(0, 0, 1), 90).toMatrix()), s_Circle, ICommandBuffer::TRANSLUCENT, s_Gizmo);
    }
}

void Handles::drawCapsule(const Vector3 &center, const Quaternion &rotation, float radius, float height) {
    if(ICommandBuffer::isInited()) {
        s_Buffer->setColor(s_Color);

        Matrix4 transform(center, rotation, Vector3(1.0));

        float half = height * 0.5f - radius;

        {
            Vector3 cap(0, half, 0);
            s_Buffer->drawMesh(transform * Matrix4(cap, Quaternion(), Vector3(radius)), s_Circle, ICommandBuffer::TRANSLUCENT, s_Gizmo);
            s_Buffer->drawMesh(transform * Matrix4(cap, Quaternion(Vector3(-90,  0, 0)), Vector3(radius)), s_Arc, ICommandBuffer::TRANSLUCENT, s_Gizmo);
            s_Buffer->drawMesh(transform * Matrix4(cap, Quaternion(Vector3(-90, 90, 0)), Vector3(radius)), s_Arc, ICommandBuffer::TRANSLUCENT, s_Gizmo);
        }
        {
            Vector3 cap(0,-half, 0);
            s_Buffer->drawMesh(transform * Matrix4(cap, Quaternion(), Vector3(radius)), s_Circle, ICommandBuffer::TRANSLUCENT, s_Gizmo);
            s_Buffer->drawMesh(transform * Matrix4(cap, Quaternion(Vector3( 90,  0, 0)), Vector3(radius)), s_Arc, ICommandBuffer::TRANSLUCENT, s_Gizmo);
            s_Buffer->drawMesh(transform * Matrix4(cap, Quaternion(Vector3( 90, 90, 0)), Vector3(radius)), s_Arc, ICommandBuffer::TRANSLUCENT, s_Gizmo);
        }

        Vector3Vector points = { Vector3( radius, half, 0),
                                 Vector3( radius,-half, 0),
                                 Vector3(-radius, half, 0),
                                 Vector3(-radius,-half, 0),
                                 Vector3( 0, half, radius),
                                 Vector3( 0,-half, radius),
                                 Vector3( 0, half,-radius),
                                 Vector3( 0,-half,-radius)};

        Mesh::IndexVector indices = {0, 1, 2, 3, 4, 5, 6, 7};

        drawLines(transform, points, indices);
    }
}

bool Handles::drawBillboard(const Vector3 &position, const Vector2 &size, Texture *texture) {
    bool result = false;
    if(ICommandBuffer::isInited()) {
        Matrix4 model(position, Quaternion(), Vector3(size, size.x));
        Matrix4 q   = model * Matrix4(Camera::current()->actor()->transform()->rotation().toMatrix());

        if(HandleTools::distanceToPoint(q, Vector3()) <= sense) {
            result = true;
        }

        s_Sprite->setTexture(OVERRIDE,  texture);
        if(s_Buffer) {
            s_Buffer->setColor(s_Color);
            s_Buffer->drawMesh(q, s_Quad, ICommandBuffer::TRANSLUCENT, s_Sprite);
        }
    }
    return result;
}

Vector3 Handles::moveTool(const Vector3 &position, const Quaternion &rotation, bool locked) {
    Vector3 result = position;
    if(ICommandBuffer::isInited()) {
        Camera *camera  = Camera::current();
        if(camera) {
            Vector3 normal = position - camera->actor()->transform()->position();
            float scale = 1.0f;
            if(!camera->orthographic()) {
                scale = normal.length();
            } else {
                scale = camera->orthoHeight();
            }
            scale *= (CONTROL_SIZE / m_sScreen.y);
            Matrix4 model(position, rotation, scale);

            Matrix4 x   = model * Matrix4(Vector3(conesize, 0, 0), Quaternion(Vector3(0, 0, 1),-90) * Quaternion(Vector3(0, 1, 0),-90), conesize);
            Matrix4 y   = model * Matrix4(Vector3(0, conesize, 0), Quaternion(), conesize);
            Matrix4 z   = model * Matrix4(Vector3(0, 0, conesize), Quaternion(Vector3(0, 0, 1), 90) * Quaternion(Vector3(1, 0, 0), 90), conesize);

            Matrix4 r(Vector3(), Quaternion(Vector3(0, 1, 0),-90), Vector3(1));

            if(!locked) {
                if(HandleTools::distanceToPoint(model, Vector3()) <= sense) {
                    s_Axes  = AXIS_X | AXIS_Y | AXIS_Z;
                } else if((HandleTools::distanceToMesh(x, s_Move) <= sense) ||
                          (HandleTools::distanceToMesh(z * r, s_Move) <= sense)) {
                    s_Axes  = AXIS_X | AXIS_Z;
                } else if((HandleTools::distanceToMesh(y, s_Move) <= sense) ||
                          (HandleTools::distanceToMesh(x * r, s_Move) <= sense)) {
                    s_Axes  = AXIS_Y | AXIS_X;
                } else if((HandleTools::distanceToMesh(z, s_Move) <= sense) ||
                          (HandleTools::distanceToMesh(y * r, s_Move) <= sense)) {
                    s_Axes  = AXIS_Z | AXIS_Y;
                } else if(HandleTools::distanceToMesh(x, s_Axis) <= sense) {
                    s_Axes  = AXIS_X;
                } else if(HandleTools::distanceToMesh(y, s_Axis) <= sense) {
                    s_Axes  = AXIS_Y;
                } else if(HandleTools::distanceToMesh(z, s_Axis) <= sense) {
                    s_Axes  = AXIS_Z;
                }
            }

            s_Second    = s_xColor;
            s_Color     = (s_Axes & AXIS_X) ? s_Selected : s_xColor;
            drawArrow(x);
            s_Buffer->setColor(s_Axes == (AXIS_X | AXIS_Z) ? s_Selected : s_xColor);
            s_Buffer->drawMesh(x, s_Move, ICommandBuffer::TRANSLUCENT, s_Gizmo);
            s_Buffer->setColor(s_Axes == (AXIS_X | AXIS_Y) ? s_Selected : s_xColor);
            s_Buffer->drawMesh(x * r, s_Move, ICommandBuffer::TRANSLUCENT, s_Gizmo);

            s_Second    = s_yColor;
            s_Color     = (s_Axes & AXIS_Y) ? s_Selected : s_yColor;
            drawArrow(y);
            s_Buffer->setColor(s_Axes == (AXIS_X | AXIS_Y) ? s_Selected : s_yColor);
            s_Buffer->drawMesh(y, s_Move, ICommandBuffer::TRANSLUCENT, s_Gizmo);
            s_Buffer->setColor(s_Axes == (AXIS_Y | AXIS_Z) ? s_Selected : s_yColor);
            s_Buffer->drawMesh(y * r, s_Move, ICommandBuffer::TRANSLUCENT, s_Gizmo);

            s_Second    = s_zColor;
            s_Color     = (s_Axes & AXIS_Z) ? s_Selected : s_zColor;
            drawArrow(z);
            s_Buffer->setColor(s_Axes == (AXIS_Y | AXIS_Z) ? s_Selected : s_zColor);
            s_Buffer->drawMesh(z, s_Move, ICommandBuffer::TRANSLUCENT, s_Gizmo);
            s_Buffer->setColor(s_Axes == (AXIS_X | AXIS_Z) ? s_Selected : s_zColor);
            s_Buffer->drawMesh(z * r, s_Move, ICommandBuffer::TRANSLUCENT, s_Gizmo);

            s_Color = s_Selected;
            s_Color.w = ALPHA;
            if(s_Axes == (AXIS_X | AXIS_Z)) {
                s_Buffer->setColor(s_Color);
                s_Buffer->drawMesh(x, s_MoveXY, ICommandBuffer::TRANSLUCENT, s_Gizmo);
            }
            if(s_Axes == (AXIS_X | AXIS_Y)) {
                s_Buffer->setColor(s_Color);
                s_Buffer->drawMesh(y, s_MoveXY, ICommandBuffer::TRANSLUCENT, s_Gizmo);
            }
            if(s_Axes == (AXIS_Y | AXIS_Z)) {
                s_Buffer->setColor(s_Color);
                s_Buffer->drawMesh(z, s_MoveXY, ICommandBuffer::TRANSLUCENT, s_Gizmo);
            }
            s_Color     = s_Normal;
            s_Second    = s_Normal;

            Plane plane;
            plane.point     = position;
            plane.normal    = camera->actor()->transform()->rotation() * Vector3(0.0f, 0.0f, 1.0f);
            if(s_Axes == AXIS_X) {
                plane.normal = Vector3(0.0f, plane.normal.y, plane.normal.z);
            } else if(s_Axes == AXIS_Z) {
                plane.normal = Vector3(plane.normal.x, plane.normal.y, 0.0f);
            } else if(s_Axes == (AXIS_X | AXIS_Z)) {
                plane.normal = Vector3(0.0f, 1.0f, 0.0f);
            } else if(s_Axes == (AXIS_X | AXIS_Y)) {
                plane.normal = Vector3(0.0f, 0.0f, 1.0f);
            } else if(s_Axes == (AXIS_Z | AXIS_Y)) {
                plane.normal = Vector3(1.0f, 0.0f, 0.0f);
            } else if(s_Axes == AXIS_Y || s_Axes == (AXIS_X | AXIS_Y | AXIS_Z)) {
                plane.normal = Vector3(plane.normal.x, 0.0f, plane.normal.z);
            }
            plane.d = plane.normal.dot(plane.point);

            Ray ray = camera->castRay(m_sMouse.x, m_sMouse.y);
            Vector3 point;
            ray.intersect(plane, &point, true);
            if(s_Axes & AXIS_X) {
                result.x = point.x;
            }
            if(s_Axes & AXIS_Y) {
                result.y = point.y;
            }
            if(s_Axes & AXIS_Z) {
                result.z = point.z;
            }
        }
    }
    return result;
}

Vector3 Handles::rotationTool(const Vector3 &position, const Quaternion &rotation, bool locked, float angle) {
    if(ICommandBuffer::isInited()) {
        Camera *camera  = Camera::current();
        if(camera) {
            Transform *t = camera->actor()->transform();
            Vector3 normal = position - t->position();
            float scale = 1.0f;
            if(!camera->orthographic()) {
                scale = normal.length();
            } else {
                scale = camera->orthoHeight();
            }
            scale *= (CONTROL_SIZE / m_sScreen.y);
            normal.normalize();

            Matrix4 model(position, rotation, scale * 5.0f);

            Matrix4 q1  = model * Matrix4(Vector3(), t->rotation() * Quaternion(Vector3(90, 0, 0)), Vector3(conesize));

            Matrix4 x   = model * Matrix4(Vector3(), Quaternion(Vector3(0, 0, 90)) *
                                                     Quaternion(Vector3(0, 1, 0), RAD2DEG * atan2(normal.y, normal.z) + 180), Vector3(conesize));
            Matrix4 y   = model * Matrix4(Vector3(), Quaternion(Vector3(0, 1, 0), RAD2DEG * atan2(normal.x, normal.z) + 180), Vector3(conesize));
            Matrix4 z   = model * Matrix4(Vector3(), Quaternion(Vector3(0, 0, 1),-RAD2DEG * atan2(normal.x, normal.y)) *
                                                     Quaternion(Vector3(90, 0, 0)), Vector3(conesize));

            Matrix4 m;
            m.scale(1.2f);

            if(!locked) {
                if(HandleTools::distanceToMesh(q1 * m, s_Circle) <= sense) {
                    s_Axes  = AXIS_X | AXIS_Y | AXIS_Z;
                } else if(HandleTools::distanceToMesh(x, s_Arc) <= sense) {
                    s_Axes  = AXIS_X;
                } else if(HandleTools::distanceToMesh(y, s_Arc) <= sense) {
                    s_Axes  = AXIS_Y;
                } else if(HandleTools::distanceToMesh(z, s_Arc) <= sense) {
                    s_Axes  = AXIS_Z;
                }
            }

            float start = 0.0f;

            if(locked) {
                s_Color = s_Selected;
                s_Color.w = ALPHA;
                if(s_Axes == (AXIS_X | AXIS_Y | AXIS_Z)) {
                    drawDisk(position, Quaternion(Vector3(90, 0, 0)), scale, start, -angle);
                    s_Buffer->setColor(s_Selected);
                    s_Buffer->drawMesh(q1 * m, s_Circle, ICommandBuffer::TRANSLUCENT, s_Gizmo);
                } else if(s_Axes == AXIS_X) {
                    drawDisk(position, Quaternion(Vector3(0, 0, 1), 90), scale, start, angle);
                    s_Buffer->setColor(s_Selected);
                    s_Buffer->drawMesh(x, s_Circle, ICommandBuffer::TRANSLUCENT, s_Gizmo);
                } else if(s_Axes == AXIS_Y) {
                    drawDisk(position, Quaternion(), scale, start, -angle);
                    s_Buffer->setColor(s_Selected);
                    s_Buffer->drawMesh(y, s_Circle, ICommandBuffer::TRANSLUCENT, s_Gizmo);
                } else if(s_Axes == AXIS_Z) {
                    drawDisk(position, Quaternion(Vector3(1, 0, 0), 90), scale, start, -angle);
                    s_Buffer->setColor(s_Selected);
                    s_Buffer->drawMesh(z, s_Circle, ICommandBuffer::TRANSLUCENT, s_Gizmo);
                }
            } else {
                s_Color = (s_Axes == AXIS_X) ? s_Selected : s_xColor;
                s_Buffer->setColor(s_Color);
                s_Buffer->drawMesh(x, s_Arc, ICommandBuffer::TRANSLUCENT, s_Gizmo);

                s_Color = (s_Axes == AXIS_Y) ? s_Selected : s_yColor;
                s_Buffer->setColor(s_Color);
                s_Buffer->drawMesh(y, s_Arc, ICommandBuffer::TRANSLUCENT, s_Gizmo);

                s_Color = (s_Axes == AXIS_Z) ? s_Selected : s_zColor;
                s_Buffer->setColor(s_Color);
                s_Buffer->drawMesh(z, s_Arc, ICommandBuffer::TRANSLUCENT, s_Gizmo);
            }

            s_Color = (s_Axes == (AXIS_X | AXIS_Y | AXIS_Z)) ? s_Selected : grey * 2.0f;
            s_Buffer->setColor(s_Color);
            s_Buffer->drawMesh(q1 * m, s_Circle, ICommandBuffer::TRANSLUCENT, s_Gizmo);
            s_Buffer->setColor(grey);
            s_Buffer->drawMesh(q1, s_Circle, ICommandBuffer::TRANSLUCENT, s_Gizmo);

            s_Color = s_Normal;
            s_Buffer->setColor(s_Color);
        }
    }
    return Vector3(m_sMouse, 1.0) * 500;
}

Vector3 Handles::scaleTool(const Vector3 &position, const Quaternion &rotation, bool locked) {
    if(ICommandBuffer::isInited()) {
        Camera *camera  = Camera::current();
        if(camera) {
            Vector3 normal = position - camera->actor()->transform()->position();
            float size = 1.0f;
            if(!camera->orthographic()) {
                size = normal.length();
            } else {
                size = camera->orthoHeight();
            }
            size *= (CONTROL_SIZE / m_sScreen.y);
            Vector3 scale(((normal.x < 0) ? 1 : -1) * size,
                          ((normal.y < 0) ? 1 : -1) * size,
                          ((normal.z < 0) ? 1 : -1) * size);

            Matrix4 model(position, rotation, scale);
            Matrix4 x   = model * Matrix4(Vector3(), Quaternion(Vector3(0, 0, 1),-90) * Quaternion(Vector3(0, 1, 0),-90), Vector3(conesize));
            Matrix4 y   = model * Matrix4(Vector3(), Quaternion(), Vector3(conesize));
            Matrix4 z   = model * Matrix4(Vector3(), Quaternion(Vector3(0, 0, 1), 90) * Quaternion(Vector3(1, 0, 0), 90), Vector3(conesize));

            Matrix4 r(Vector3(), Quaternion(Vector3(0, 1, 0),-90), Vector3(1));

            if(!locked) {
                if(HandleTools::distanceToPoint(model, Vector3()) <= sense) {
                    s_Axes  = AXIS_X | AXIS_Y | AXIS_Z;
                } else if((HandleTools::distanceToMesh(x, s_Scale) <= sense) ||
                          (HandleTools::distanceToMesh(z * r, s_Scale) <= sense)) {
                    s_Axes  = AXIS_X | AXIS_Z;
                } else if((HandleTools::distanceToMesh(y, s_Scale) <= sense) ||
                          (HandleTools::distanceToMesh(x * r, s_Scale) <= sense)) {
                    s_Axes  = AXIS_Y | AXIS_X;
                } else if((HandleTools::distanceToMesh(z, s_Scale) <= sense) ||
                          (HandleTools::distanceToMesh(y * r, s_Scale) <= sense)) {
                    s_Axes  = AXIS_Z | AXIS_Y;
                } else if(HandleTools::distanceToMesh(x, s_Axis) <= sense) {
                    s_Axes  = AXIS_X;
                } else if(HandleTools::distanceToMesh(y, s_Axis) <= sense) {
                    s_Axes  = AXIS_Y;
                } else if(HandleTools::distanceToMesh(z, s_Axis) <= sense) {
                    s_Axes  = AXIS_Z;
                }
            }

            s_Color     = s_Selected;
            s_Color.w   = ALPHA;
            {
                s_Buffer->setColor(s_xColor);
                if(s_Axes == (AXIS_X | AXIS_Z)) {
                    s_Buffer->setColor(s_Color);
                    s_Buffer->drawMesh(x, s_ScaleXY, ICommandBuffer::TRANSLUCENT, s_Gizmo);
                    s_Buffer->setColor(s_Selected);
                }
                s_Buffer->drawMesh(x, s_Scale, ICommandBuffer::TRANSLUCENT, s_Gizmo);

                s_Buffer->setColor(s_xColor);
                if(s_Axes == (AXIS_X | AXIS_Y)) {
                    s_Buffer->setColor(s_Color);
                    s_Buffer->drawMesh(x * r, s_ScaleXY, ICommandBuffer::TRANSLUCENT, s_Gizmo);
                    s_Buffer->setColor(s_Selected);
                }
                s_Buffer->drawMesh(x * r, s_Scale, ICommandBuffer::TRANSLUCENT, s_Gizmo);

                s_Buffer->setColor((s_Axes & AXIS_X) ? s_Selected : s_xColor);
                s_Buffer->drawMesh(x, s_Axis, ICommandBuffer::TRANSLUCENT, s_Gizmo);
            }
            {
                s_Buffer->setColor(s_yColor);
                if(s_Axes == (AXIS_Y | AXIS_X)) {
                    s_Buffer->setColor(s_Color);
                    s_Buffer->drawMesh(y, s_ScaleXY, ICommandBuffer::TRANSLUCENT, s_Gizmo);
                    s_Buffer->setColor(s_Selected);
                }
                s_Buffer->drawMesh(y, s_Scale, ICommandBuffer::TRANSLUCENT, s_Gizmo);

                s_Buffer->setColor(s_yColor);
                if(s_Axes == (AXIS_Y | AXIS_Z)) {
                    s_Buffer->setColor(s_Color);
                    s_Buffer->drawMesh(y * r, s_ScaleXY, ICommandBuffer::TRANSLUCENT, s_Gizmo);
                    s_Buffer->setColor(s_Selected);
                }
                s_Buffer->drawMesh(y * r, s_Scale, ICommandBuffer::TRANSLUCENT, s_Gizmo);

                s_Buffer->setColor((s_Axes & AXIS_Y) ? s_Selected : s_yColor);
                s_Buffer->drawMesh(y, s_Axis, ICommandBuffer::TRANSLUCENT, s_Gizmo);
            }
            {
                s_Buffer->setColor(s_zColor);
                if(s_Axes == (AXIS_Z | AXIS_Y)) {
                    s_Buffer->setColor(s_Color);
                    s_Buffer->drawMesh(z, s_ScaleXY, ICommandBuffer::TRANSLUCENT, s_Gizmo);
                    s_Buffer->setColor(s_Selected);
                }
                s_Buffer->drawMesh(z, s_Scale, ICommandBuffer::TRANSLUCENT, s_Gizmo);

                s_Buffer->setColor(s_zColor);
                if(s_Axes == (AXIS_Z | AXIS_X)) {
                    s_Buffer->setColor(s_Color);
                    s_Buffer->drawMesh(z * r, s_ScaleXY, ICommandBuffer::TRANSLUCENT, s_Gizmo);
                    s_Buffer->setColor(s_Selected);
                }
                s_Buffer->drawMesh(z * r, s_Scale, ICommandBuffer::TRANSLUCENT, s_Gizmo);

                s_Buffer->setColor((s_Axes & AXIS_Z) ? s_Selected : s_zColor);
                s_Buffer->drawMesh(z, s_Axis, ICommandBuffer::TRANSLUCENT, s_Gizmo);
            }

            if(s_Axes == (AXIS_X | AXIS_Y | AXIS_Z)) {
                s_Buffer->setColor(s_Color);
                s_Buffer->drawMesh(x,       s_ScaleXYZ, ICommandBuffer::TRANSLUCENT, s_Gizmo);
                s_Buffer->drawMesh(x * r,   s_ScaleXYZ, ICommandBuffer::TRANSLUCENT, s_Gizmo);
                s_Buffer->drawMesh(y,       s_ScaleXYZ, ICommandBuffer::TRANSLUCENT, s_Gizmo);
                s_Buffer->drawMesh(y * r,   s_ScaleXYZ, ICommandBuffer::TRANSLUCENT, s_Gizmo);
                s_Buffer->drawMesh(z,       s_ScaleXYZ, ICommandBuffer::TRANSLUCENT, s_Gizmo);
                s_Buffer->drawMesh(z * r,   s_ScaleXYZ, ICommandBuffer::TRANSLUCENT, s_Gizmo);
            }
            s_Color = s_Normal;
        }
    }
    return Vector3(m_sMouse, 1.0) * 500;
}


