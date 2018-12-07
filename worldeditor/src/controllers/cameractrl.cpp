#include "cameractrl.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QCursor>

#include <engine.h>
#include <timer.h>

#include <components/scene.h>
#include <components/camera.h>
#include <components/actor.h>
#include <components/transform.h>
#include <components/staticmesh.h>
#include <components/spritemesh.h>
#include <components/textmesh.h>

#include "graph/viewport.h"

CameraCtrl::CameraCtrl(Viewport *view) :
        IController(),
        mBlockMove(false),
        mBlockRot(false),
        mCameraFree(true),
        mCameraMove(MoveTypes::MOVE_IDLE),
        mCameraSpeed(Vector3()),
        m_pCamera(nullptr),
        m_pView(view) {
}

void CameraCtrl::init(Scene *scene) {
    m_pCamera   = Engine::objectCreate<Actor>("Camera");
    m_pActiveCamera = m_pCamera->addComponent<Camera>();
    m_pActiveCamera->setFocal(10.0f);
    m_pActiveCamera->setOrthoWidth(10.0f);
    m_pActiveCamera->setColor(Vector4(0.2f, 0.2f, 0.2f, 0.0));

    m_pCamera->transform()->setPosition(Vector3(0.0, 0.0, 20.0));
}

void CameraCtrl::update() {
    if(!mBlockMove) {
        if(mCameraMove & MOVE_FORWARD) {
            mCameraSpeed.z += 0.1f;
        }

        if(mCameraMove & MOVE_BACKWARD) {
            mCameraSpeed.z -= 0.1f;
        }

        if(mCameraMove & MOVE_LEFT) {
            mCameraSpeed.x -= 0.1f;
        }

        if(mCameraMove & MOVE_RIGHT) {
            mCameraSpeed.x += 0.1f;
        }
    }

    if( m_pActiveCamera && (mCameraSpeed.x != 0 || mCameraSpeed.y != 0 || mCameraSpeed.z != 0) ) {
        Transform *t    = m_pCamera->transform();
        Vector3 pos     = t->position();
        Vector3 dir     = t->rotation() * Vector3(0.0, 0.0, 1.0);
        dir.normalize();

        Vector3 delta = (dir * mCameraSpeed.z) + dir.cross(Vector3(0.0f, 1.0f, 0.0f)) * mCameraSpeed.x;
        t->setPosition(pos - delta * m_pActiveCamera->focal() * 0.1f);

        mCameraSpeed   -= mCameraSpeed * 10.0f * Timer::deltaTime();
        if(mCameraSpeed.length() <= .01f) {
            mCameraSpeed    = Vector3();
        }
    }
}

void CameraCtrl::drawHandles() {

}

void CameraCtrl::onOrthographic(bool flag) {
    if(m_pActiveCamera->orthographic() != flag) {
        Transform *t    = m_pCamera->transform();
        if(flag) {
            mRotation   = t->rotation();
            t->setRotation(Quaternion());
        } else {
            t->setRotation(mRotation);
        }

        m_pActiveCamera->setOrthographic(flag);
    }
}

void CameraCtrl::setFocusOn(Actor *actor, float &bottom) {
    bottom  = 0;
    if(actor) {
        Vector3 pos;
        float radius    = 0;
        Transform *t    = actor->transform();
        for(auto it : actor->getChildren()) {
            Mesh *mesh  = nullptr;
            /// \todo Bad switch case
            StaticMesh *staticMesh  = dynamic_cast<StaticMesh *>(it);
            if(staticMesh) {
                mesh    = staticMesh->mesh();
            } else {
                SpriteMesh *spriteMesh  = dynamic_cast<SpriteMesh *>(it);
                if(spriteMesh) {
                    mesh    = spriteMesh->mesh();
                } else {
                    TextMesh *textMesh  = dynamic_cast<TextMesh *>(it);
                    if(textMesh) {
                        mesh    = textMesh->mesh();
                    }
                }
            }

            if(mesh) {
                radius = 0;
                uint32_t i  = 0;
                for(uint32_t s = 0; s < mesh->surfacesCount(); s++) {
                    AABBox aabb = mesh->bound(s);
                    Vector3 scale   = t->worldScale();
                    pos    += aabb.center * scale;
                    radius += (aabb.size * scale).length();
                    Vector3 min, max;
                    aabb.box(min, max);
                    if(i == 0) {
                        bottom  = min.y;
                    }
                    bottom = MIN(bottom, min.y);
                    i++;
                }
                uint32_t size   = mesh->surfacesCount();
                pos    /= size;
                radius /= size;
                radius /= sinf(m_pActiveCamera->fov() * DEG2RAD);
            } else {
                radius = 1.0f;
            }

        }

        m_pActiveCamera->setFocal(radius);
        m_pActiveCamera->setOrthoWidth(radius);
        Transform *camera   = m_pCamera->transform();
        camera->setPosition(t->worldPosition() + pos + camera->rotation() * Vector3(0.0, 0.0, radius));
    }
}

void CameraCtrl::onInputEvent(QInputEvent *pe) {
    switch(pe->type()) {
        case QEvent::KeyPress: {
            QKeyEvent *e    = static_cast<QKeyEvent *>(pe);
            switch(e->key()) {
                case Qt::Key_W:
                case Qt::Key_Up: {
                    mCameraMove |= MOVE_FORWARD;
                } break;
                case Qt::Key_S:
                case Qt::Key_Down: {
                    mCameraMove |= MOVE_BACKWARD;
                } break;
                case Qt::Key_A:
                case Qt::Key_Left: {
                    mCameraMove |= MOVE_LEFT;
                } break;
                case Qt::Key_D:
                case Qt::Key_Right: {
                    mCameraMove |= MOVE_RIGHT;
                } break;
                default: break;
            }
        } break;
        case QEvent::KeyRelease: {
            QKeyEvent *e    = static_cast<QKeyEvent *>(pe);
            switch(e->key()) {
                case Qt::Key_W:
                case Qt::Key_Up: {
                    mCameraMove &= ~MOVE_FORWARD;
                } break;
                case Qt::Key_S:
                case Qt::Key_Down: {
                    mCameraMove &= ~MOVE_BACKWARD;
                } break;
                case Qt::Key_A:
                case Qt::Key_Left: {
                    mCameraMove &= ~MOVE_LEFT;
                } break;
                case Qt::Key_D:
                case Qt::Key_Right: {
                    mCameraMove &= ~MOVE_RIGHT;
                } break;
                default: break;
            }
        } break;
        case QEvent::MouseButtonPress: {
            QMouseEvent *e  = static_cast<QMouseEvent *>(pe);
            if(e->buttons() & Qt::RightButton) {
                mSaved  = e->globalPos();
            }
        } break;
        case QEvent::MouseMove: {
            QMouseEvent *e  = static_cast<QMouseEvent *>(pe);
            QPoint pos      = e->globalPos();
            QPoint delta    = pos - mSaved;

            if(e->buttons() & Qt::RightButton) {
                if(m_pActiveCamera->orthographic()) {
                    cameraMove(Vector3((float)delta.x() / (float)m_pView->width(),
                                     -((float)delta.y() / ((float)m_pView->height() * m_pActiveCamera->ratio())), 0.0f));
                } else {
                    if(!mBlockRot)  {
                        cameraRotate(Vector3(delta.y(), delta.x(), 0.0f) * 0.1f);
                    }
                }

                QRect r = QApplication::desktop()->screenGeometry(m_pView);
                if(pos.x() >= r.right()) {
                    pos.setX(pos.x() - r.width());
                }
                if(pos.x() <= r.left()) {
                    pos.setX(pos.x() + r.width());
                }

                if(pos.y() >= r.bottom()) {
                    pos.setY(pos.y() - r.height() + 2);
                }
                if(pos.y() <= r.top()) {
                    pos.setY(pos.y() + r.height() - 2);
                }
                QCursor::setPos(pos);
            }
            mSaved  = pos;
        } break;
        case QEvent::Wheel: {
            cameraZoom(static_cast<QWheelEvent *>(pe)->delta() * 0.01f);
        } break;
        default: break;
    }
}

void CameraCtrl::cameraZoom(float delta) {
    if(m_pActiveCamera && m_pCamera) {
        float focal = m_pActiveCamera->focal() - delta;
        if(focal > 0.0f) {
            m_pActiveCamera->setFocal(focal);
            m_pActiveCamera->setOrthoWidth(focal);

            Transform *t    = m_pCamera->transform();

            t->setPosition(t->position() - t->rotation() * Vector3(0.0, 0.0, delta));
        }
    }
}

void CameraCtrl::cameraRotate(const Vector3 &delta) {
    Transform *t    = m_pCamera->transform();
    Vector3 euler   = t->euler() - delta;
    mRotation       = Quaternion(euler);

    Vector3 target  = t->position() - t->rotation() * Vector3(0.0, 0.0, m_pActiveCamera->focal());
    if(!mCameraFree) {
        t->setPosition(target + mRotation * Vector3(0.0, 0.0, m_pActiveCamera->focal()));
    }
    t->setEuler(euler);
}

void CameraCtrl::cameraMove(const Vector3 &delta) {
    Transform *t    = m_pCamera->transform();
    t->setPosition(t->position() - delta * m_pActiveCamera->focal());
}
