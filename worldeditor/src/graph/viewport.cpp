#include "viewport.h"

#include <system.h>
#include <commandbuffer.h>

#include <components/camera.h>
#include <components/scene.h>

#include <resources/pipeline.h>
#include <resources/material.h>
#include <resources/rendertexture.h>

#include <editor/handles.h>

#include "controllers/objectctrl.h"

#include "commandbuffer.h"

#include "pluginmanager.h"

#define OVERRIDE "uni.texture0"

Viewport::Viewport(QWidget *parent) :
        QOpenGLWidget(parent),
        m_pController(nullptr) {

    setAcceptDrops(true);
    //setContextMenuPolicy(Qt::CustomContextMenu);
    setAutoFillBackground(false);

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

void Viewport::setController(CameraCtrl *ctrl) {
    m_pController = ctrl;
}

void Viewport::initializeGL() {
    QOpenGLWidget::initializeGL();

    emit inited();
}

void Viewport::setScene(Scene *scene) {
    m_pScene = scene;
    PluginManager::instance()->addScene(m_pScene);
}

void Viewport::paintGL() {
    if(m_pController) {
        m_pController->update();
    }
    if(m_pScene) {
        findCamera();

        PluginManager::instance()->updateRender(m_pScene);
    }
}

void Viewport::resizeGL(int width, int height) {
    QOpenGLWidget::resizeGL(width, height);

    Camera *camera = Camera::current();
    if(camera) {
        Pipeline *pipe = camera->pipeline();
        pipe->resize(width, height);
        pipe->setTarget(defaultFramebufferObject());
    }

    if(m_pController) {
        Camera::setCurrent(m_pController->camera());
    }
}

void Viewport::dragEnterEvent(QDragEnterEvent *event) {
    emit dragEnter(event);
}

void Viewport::dragMoveEvent(QDragMoveEvent *event) {
    emit dragMove(event);
}

void Viewport::dragLeaveEvent(QDragLeaveEvent *event) {
    emit dragLeave(event);
}

void Viewport::dropEvent(QDropEvent *event) {
    setFocus();
    emit drop(event);
}

void Viewport::mouseMoveEvent(QMouseEvent *pe) {
    if(m_pController) {
        static_cast<CameraCtrl *>(m_pController)->onInputEvent(pe);
    }
}

void Viewport::mousePressEvent(QMouseEvent *pe) {
    if(m_pController) {
        static_cast<CameraCtrl *>(m_pController)->onInputEvent(pe);
    }
}

void Viewport::mouseReleaseEvent(QMouseEvent *pe) {
    if(m_pController) {
        static_cast<CameraCtrl *>(m_pController)->onInputEvent(pe);
    }
}

void Viewport::wheelEvent(QWheelEvent *pe) {
    if(m_pController) {
        static_cast<CameraCtrl *>(m_pController)->onInputEvent(pe);
    }
}

void Viewport::keyPressEvent(QKeyEvent *pe) {
    QWidget::keyPressEvent(pe);
    if(m_pController) {
        static_cast<CameraCtrl *>(m_pController)->onInputEvent(pe);
    }
}

void Viewport::keyReleaseEvent(QKeyEvent *pe) {
    QWidget::keyReleaseEvent(pe);
    if(m_pController) {
        static_cast<CameraCtrl *>(m_pController)->onInputEvent(pe);
    }
}

void Viewport::findCamera() {
    if(m_pController) {
        Camera *camera = m_pController->camera();
        if(camera) {
            Pipeline *pipe = camera->pipeline();
            pipe->resize(width(), height());
            pipe->setTarget(defaultFramebufferObject());
        }
        Camera::setCurrent(camera);
    }
}
