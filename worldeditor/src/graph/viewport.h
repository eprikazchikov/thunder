#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "sceneview.h"

class Viewport : public SceneView {
    Q_OBJECT
public:
    Viewport                (QWidget *parent = 0);

protected:
    void                    initializeGL        ();
    void                    paintGL             ();
    void                    resizeGL            (int width, int height);

signals:
    void                    drop                (QDropEvent *);
    void                    dragEnter           (QDragEnterEvent *);
    void                    dragMove            (QDragMoveEvent *);
    void                    dragLeave           (QDragLeaveEvent *);

protected:
    void                    dragEnterEvent      (QDragEnterEvent *);
    void                    dragMoveEvent       (QDragMoveEvent *);
    void                    dragLeaveEvent      (QDragLeaveEvent *);
    void                    dropEvent           (QDropEvent *);

    void                    mouseMoveEvent      (QMouseEvent *);
    void                    mousePressEvent     (QMouseEvent *);
    void                    mouseReleaseEvent   (QMouseEvent *);

    void                    wheelEvent          (QWheelEvent *);

    void                    keyPressEvent       (QKeyEvent *);
    void                    keyReleaseEvent     (QKeyEvent *);

private:
    void                    findCamera          ();

};

#endif // VIEWPORT_H
