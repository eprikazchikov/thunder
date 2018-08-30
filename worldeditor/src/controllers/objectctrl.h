#ifndef OBJECTCTRL_H
#define OBJECTCTRL_H

#include <QObject>
#include <QInputEvent>

#include <cstdint>
#include <map>

#include <amath.h>
#include <object.h>

#include "graph/viewport.h"

#include "cameractrl.h"
#include "managers/undomanager/undomanager.h"

class Engine;
class Actor;
class Scene;

class ObjectCtrl : public CameraCtrl {
    Q_OBJECT

public:
    enum ModeTypes {
        MODE_TRANSLATE  = 1,
        MODE_ROTATE     = 2,
        MODE_SCALE      = 3
    };

    struct Select {
        Actor          *object;
        Vector3         position;
        Vector3         scale;
        Vector3         euler;
    };

public:
    ObjectCtrl          (Viewport *view);

    void                drawHandles                 ();

    void                clear                       (bool signal = true);

    void                deleteSelected              (bool force = false);

    void                selectActor                 (const list<uint32_t> &list, bool undo = true);

    Object::ObjectList  selected                    ();

    void                setMap                      (Object *map)   { m_pMap = map; }

    void                setMoveGrid                 (float value)   { mMoveGrid = value; }
    void                setAngleGrid                (float value)   { mAngleGrid = value; }

    Object             *findObject                  (uint32_t id, Object *parent = nullptr);

    void                resize                      (uint32_t width, uint32_t height);

public slots:
    void                onInputEvent                (QInputEvent *);

    void                onComponentSelected         (const QString &path);

    void                onDrop                      ();
    void                onDragEnter                 (QDragEnterEvent *);
    void                onDragLeave                 (QDragLeaveEvent *);

    void                onSelectActor               (Object::ObjectList list, bool undo = true);
    void                onRemoveActor               (Object::ObjectList, bool undo = true);
    void                onParentActor               (Object::ObjectList objects, Object::ObjectList parents, bool undo = true);

    void                onFocusActor                (Object *object);

    void                onMoveActor                 ();
    void                onRotateActor               ();
    void                onScaleActor                ();

signals:
    void                mapUpdated                  ();

    void                objectsUpdated              ();

    void                objectsChanged              (Object::ObjectList objects, const QString &property);

    void                objectsSelected             (Object::ObjectList objects);

    void                loadMap                     (const QString &map);

protected:
    void                drawHelpers                 (Object &object);

    void                selectGeometry              (Vector2 &, Vector2 &);

    Vector3             objectPosition              ();

    bool                isDrag                      ()  { return mDrag; }

    void                setDrag                     (bool drag);

private slots:


protected:
    typedef map<uint32_t, Select>   SelectMap;
    SelectMap           m_Selected;

    bool                mAdditive;
    bool                mCopy;
    bool                mDrag;

    /// Current mode (see AController::ModeTypes)
    uint8_t             mMode;

    uint8_t             mAxes;

    float               mMoveGrid;
    float               mAngleGrid;
    float               mScaleGrid;

    Object             *m_pMap;

    Object::ObjectList  m_DragObjects;

    QString             m_DragMap;

    Vector2             mMousePosition;
    Vector2             m_Screen;

    Vector3             mWorld;
    Vector3             mSaved;
    Vector3             mPosition;

    UndoManager::PropertyObjects   *m_pPropertyState;
};

#endif // OBJECTCTRL_H
