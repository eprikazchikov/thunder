#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#include <QOpenGLWidget>

#include <QInputEvent>
#include <QMenu>
#include <QPainter>

#include <components/scene.h>
#include <adapters/iplatformadaptor.h>
#include "config.h"

class Engine;
class ISystem;
class ICommandBuffer;

class CameraCtrl;

class QOffscreenSurface;
class QOpenGLFramebufferObject;

class SceneView : public QOpenGLWidget, public IPlatformAdaptor {
    Q_OBJECT
public:
    SceneView               (QWidget *parent = nullptr);

    ~SceneView              ();

    void                    setRender           (const QString &render);

    void                    setScene            (Scene *scene);
    Scene                  *scene               ()              { return m_pScene; }

    void                    setController       (IController *ctrl);
    IController            *controller          () const        { return m_pController; }

public:
    bool                    init                        () { return true; }

    void                    update                      () {}

    bool                    start                       () { return true; }

    void                    stop                        () {}

    void                    destroy                     () {}

    bool                    isValid                     () { return true; }

    bool                    key                         (Input::KeyCode) { return false; }

    Vector4                 mousePosition               () {
        QPoint p    = mapFromGlobal(QCursor::pos());
        return Vector4(p.x(), height() - p.y(),
                       (float)p.x() / width(), 1.0f - (float)p.y() / height());
    }

    Vector4                 mouseDelta                  () { return Vector4(); }

    uint8_t                 mouseButtons                () { return m_MouseButtons; }

    uint32_t                screenWidth                 () { return width(); }

    uint32_t                screenHeight                () { return height(); }

    void                    setMousePosition            (const Vector3 &position) {
        QCursor::setPos(mapToGlobal(QPoint(position.x, position.y)));
    }

    uint16_t                joystickCount               () { return 0; }

    uint16_t                joystickButtons             (uint8_t) { return 0; }

    Vector4                 joystickThumbs              (uint8_t) { return Vector4(); }

    Vector2                 joystickTriggers            (uint8_t) { return Vector2(); }

    void                   *pluginLoad                  (const char *) { return nullptr; }

    bool                    pluginUnload                (void *) { return false; }

    void                   *pluginAddress               (void *, const string &) { return nullptr; }

    string                  locationLocalDir            () { return string(); }

signals:
    void                    inited              ();

protected:
    void                    initializeGL        ();
    void                    paintGL             ();
    void                    resizeGL            (int width, int height);

    void                    mousePressEvent     (QMouseEvent *);
    void                    mouseReleaseEvent   (QMouseEvent *);

protected:
    virtual void            findCamera          ();

    IController            *m_pController;

    QList<ISystem *>        m_Systems;

    Scene                  *m_pScene;

    QMenu                   m_RenderModeMenu;

    int32_t                 m_MouseButtons;
};

#endif // SCENEVIEW_H
