#include "objectctrl.h"

#include <QApplication>
#include <QMimeData>
#include <QDebug>
#include <QMessageBox>

#include <components/actor.h>
#include <components/scene.h>
#include <components/camera.h>
#include <components/directlight.h>
#include <components/spritemesh.h>
#include <components/staticmesh.h>

#include "graph/handletools.h"
#include "editors/componentbrowser/componentbrowser.h"
#include "assetmanager.h"
#include "converters/converter.h"
#include "projectmanager.h"

#define DEFAULTSPRITE ".embedded/DefaultSprite.mtl"

string findFreeObjectName(const string &name, Object *parent) {
    string newName  = name;
    if(!newName.empty()) {
        Object *o   = parent->find(parent->name() + "/" + newName);
        if(o != nullptr) {
            string number;
            while(isdigit(newName.back())) {
                number.insert(0, 1, newName.back());
                newName.pop_back();
            }
            uint32_t i  = atoi(number.c_str());
            i++;
            while(parent->find(parent->name() + "/" + newName + to_string(i)) != nullptr) {
                i++;
            }
            return (newName + to_string(i));
        }
        return newName;
    }
    return "Object";
}

ObjectCtrl::ObjectCtrl(Viewport *view) :
        CameraCtrl(view) {

    mCopy       = false;
    mDrag       = false;
    mAdditive   = false;

    mMode       = ObjectCtrl::MODE_SCALE;

    mWorld      = Vector3();

    mAxes       = 0;

    mMoveGrid   = 0;
    mAngleGrid  = 0;
    mScaleGrid  = 0;

    m_pMap          = nullptr;
    m_pPropertyState= nullptr;

    mMousePosition  = Vector2();
}

void ObjectCtrl::drawHandles() {
    CameraCtrl::drawHandles();

    Handles::m_sMouse   = Vector2(mMousePosition.x / m_Screen.x, mMousePosition.y / m_Screen.y);

    drawHelpers(*m_pMap);

    if(!m_Selected.empty()) {
        switch(mMode) {
            case MODE_TRANSLATE: {
                mWorld  = Handles::moveTool(objectPosition(), mDrag);
            } break;
            case MODE_ROTATE: {
                mWorld  = Handles::rotationTool(objectPosition(), mDrag);
            } break;
            case MODE_SCALE: {
                mWorld  = Handles::scaleTool(objectPosition(), mDrag);
            } break;
            default: break;
        }
    }
}

void ObjectCtrl::clear(bool signal) {
    m_Selected.clear();
    if(signal) {
        emit objectsSelected(selected());
    }
}

void ObjectCtrl::deleteSelected(bool force) {
    m_pView->makeCurrent();
    if(!m_Selected.empty()) {
        if(force) {
            for(auto it : m_Selected) {
                delete it.second.object;
            }
        } else {
            UndoManager::instance()->push(new UndoManager::DestroyObjects(selected(), this));
        }
        clear(true);
        emit mapUpdated();
    }
}

void ObjectCtrl::drawHelpers(Object &object) {
    for(auto &it : object.getChildren()) {
        Component *component    = dynamic_cast<Component *>(it);
        if(component) {
            bool result     = false;
            Camera *camera  = dynamic_cast<Camera *>(component);
            if(camera) {
                array<Vector3, 8> a = camera->frustumCorners(camera->nearPlane(), camera->farPlane());

                Vector3Vector points(a.begin(), a.end());
                Mesh::IndexVector indices   = {0, 1, 1, 2, 2, 3, 3, 0,
                                               4, 5, 5, 6, 6, 7, 7, 4,
                                               0, 4, 1, 5, 2, 6, 3, 7};

                Handles::drawLines(component->actor().transform(), points, indices);
                result  = Handles::drawBillboard(component->actor().position(), Vector2(1.0), Engine::loadResource<Texture>(".embedded/camera.png"));
            }
            DirectLight *direct = dynamic_cast<DirectLight *>(component);
            if(direct) {
                Vector3 pos     = component->actor().position();

                Matrix4 z(Vector3(), Quaternion(Vector3(1, 0, 0),-90), Vector3(1.0));
                Handles::s_Color = Handles::s_Second = direct->color();
                Handles::drawArrow(Matrix4(pos, component->actor().rotation(), Vector3(0.5f)) * z);
                result  = Handles::drawBillboard(pos, Vector2(1.0), Engine::loadResource<Texture>(".embedded/directlight.png"));
                Handles::s_Color = Handles::s_Second = Handles::s_Normal;
            }
            if(component->typeName() == "AudioSource") {
                Vector3 pos     = component->actor().position();
                result  = Handles::drawBillboard(pos, Vector2(1.0), Engine::loadResource<Texture>(".embedded/soundsource.png"));
            }

            if(result) {
                list<uint32_t> sel;
                sel.push_back(object.uuid());
                setSelectedObjects(sel);
            }
        } else {
            drawHelpers(*it);
        }
    }
}

void ObjectCtrl::selectGeometry(Vector2 &pos, Vector2 &) {
    pos = Vector2(mMousePosition.x, mMousePosition.y);
}

Vector3 ObjectCtrl::objectPosition() {
    Vector3 result;
    if(!m_Selected.empty()) {
        for(auto &it : m_Selected) {
            result += it.second.object->worldPosition();
        }
        result  = result / m_Selected.size();
    }
    return result;
}

void ObjectCtrl::setDrag(bool drag) {
    if(drag) {
        if(mCopy) {
            m_pView->makeCurrent();
            // Making the copy of selected objects
            Object::ObjectList objects;
            for(auto it : m_Selected) {
                Actor *origin   = it.second.object;
                Actor *actor    = dynamic_cast<Actor *>(origin->clone());
                if(actor) {
                    actor->setName(findFreeObjectName(origin->name(), origin->parent()));
                    actor->setParent(origin->parent());
                    objects.push_back(actor);
                    emit mapUpdated();
                }
            }
            UndoManager::instance()->push(new UndoManager::CreateObjects(objects, this, tr("Copy Objects")));
            clear();
            onSelectActor(objects, false);
            objects.clear();
        }
        // Save params
        for(auto &it : m_Selected) {
            it.second.position  = it.second.object->position();
            it.second.scale     = it.second.object->scale();
            it.second.euler     = it.second.object->euler();
        }
        mSaved  = mWorld;
        mPosition   = objectPosition();
        m_pPropertyState    = new UndoManager::PropertyObjects(selected(), this);
    }
    mDrag   = drag;
}

Object::ObjectList ObjectCtrl::selected() {
    Object::ObjectList result;
    for(auto it : m_Selected) {
        result.push_back(it.second.object);
    }
    return result;
}

void ObjectCtrl::selectActor(const list<uint32_t> &list, bool undo) {
    bool select = list.empty();

    Object::ObjectList l;
    for(auto it : list) {
        if(m_Selected.find(it) == m_Selected.end() || !mAdditive) {
            l.push_back(findObject(it));
            select  = true;
        }
    }
    if(select) {
        onSelectActor(l, undo);
    }
}

void ObjectCtrl::onSelectActor(Object::ObjectList list, bool undo) {
    Object::ObjectList sel = selected();
    if(!mAdditive) {
        clear();
    }
    bool push   = false;
    for(auto it : list) {
        Actor *actor    = dynamic_cast<Actor *>(it);
        if(actor) {
            uint32_t id = actor->uuid();
            if(m_Selected.find(id) == m_Selected.end()) {
                push = true;
            }
            Select data;
            data.object = actor;
            m_Selected[id]  = data;
        }
    }
    if(undo && (push || list.empty())) {
        UndoManager::instance()->push( new UndoManager::SelectObjects(sel, this) );
    }

    Object::ObjectList s    = selected();
    if(!s.empty()) {
        emit objectsSelected(s);
    }
}

void ObjectCtrl::onRemoveActor(Object::ObjectList, bool undo) {
    deleteSelected(!undo);
}

void ObjectCtrl::onParentActor(Object::ObjectList objects, Object::ObjectList parents, bool undo) {
    if(undo) {
        UndoManager::instance()->push(new UndoManager::ParentingObjects(objects, parents, this));
    }

    auto parent = parents.begin();
    for(auto it : objects) {
        if(parent != parents.end()) {
            it->setParent(*parent);
            parent++;
        }
    }
    emit objectsUpdated();
    emit mapUpdated();
}

void ObjectCtrl::onFocusActor(Object *object) {
    float bottom;
    setFocusOn(dynamic_cast<Actor *>(object), bottom);
}

void ObjectCtrl::onMoveActor() {
    mMode   = MODE_TRANSLATE;
}

void ObjectCtrl::onRotateActor() {
    mMode   = MODE_ROTATE;
}

void ObjectCtrl::onScaleActor() {
    mMode   = MODE_SCALE;
}

void ObjectCtrl::onComponentSelected(const QString &path) {
    if(m_Selected.size() == 1) {
        Actor *actor    = m_Selected.begin()->second.object;
        if(actor) {
            if(actor->component(qPrintable(path)) == nullptr) {
                Component *comp = actor->addComponent(path.toStdString());
                if(comp) {
                    Object::ObjectList list;
                    list.push_back(comp);
                    UndoManager::instance()->push(new UndoManager::CreateObjects(list, this, tr("Create Component ") + path));
                    SpriteMesh *sprite  = dynamic_cast<SpriteMesh *>(comp);
                    if(sprite) {
                        sprite->setMaterial(Engine::loadResource<Material>(DEFAULTSPRITE));
                    }
                    emit objectsUpdated();
                }
            } else {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.setText("Creation Component Failed");
                msgBox.setInformativeText("Do you want to save your changes?");
                msgBox.setInformativeText(QString(tr("Component with type \"%1\" already defined for this actor.")).arg(path));
                msgBox.setStandardButtons(QMessageBox::Ok);

                msgBox.exec();
            }
        }
    }
}

void ObjectCtrl::onDrop() {
    if(!m_DragObjects.empty()) {
        UndoManager::instance()->push( new UndoManager::CreateObjects(m_DragObjects, this) );
        clear();
        emit mapUpdated();
        onSelectActor(m_DragObjects, false);
    }

    if(!m_DragMap.isEmpty()) {
        emit loadMap(ProjectManager::instance()->contentPath() + "/" + m_DragMap);
    }
}

void ObjectCtrl::onDragEnter(QDragEnterEvent *event) {
    m_DragObjects.clear();
    m_DragMap.clear();

    if(event->mimeData()->hasFormat(gMimeComponent)) {
        string name     = event->mimeData()->data(gMimeComponent).toStdString();
        Actor *actor    = Engine::objectCreate<Actor>(findFreeObjectName(name, m_pMap));
        if(actor) {
            Object *object  = Engine::objectCreate(name, findFreeObjectName(name, actor));
            Component *comp = dynamic_cast<Component *>(object);
            if(comp) {
                comp->setParent(actor);
                actor->setName(findFreeObjectName(comp->typeName(), m_pMap));
                SpriteMesh *sprite  = dynamic_cast<SpriteMesh *>(comp);
                if(sprite) {
                    sprite->setMaterial(Engine::loadResource<Material>(DEFAULTSPRITE));
                }
            } else {
                delete object;
            }
            m_DragObjects.push_back(actor);
        }
        event->acceptProposedAction();
    } else if(event->mimeData()->hasFormat(gMimeContent)) {
        event->acceptProposedAction();

        QStringList list    = QString(event->mimeData()->data(gMimeContent)).split(";");
        AssetManager *mgr   = AssetManager::instance();
        foreach(QString str, list) {
            if( !str.isEmpty() ) {
                QFileInfo info(str);
                switch(mgr->resourceType(info)) {
                    case IConverter::ContentMap: {
                        m_DragMap   = str;
                    } break;
                    case IConverter::ContentMesh: {
                        Actor *actor    = Engine::objectCreate<Actor>(findFreeObjectName(info.baseName().toStdString(), m_pMap));
                        StaticMesh *m   = actor->addComponent<StaticMesh>();
                        if(m) {
                            m->setMesh(Engine::loadResource<Mesh>( qPrintable(str) ));
                        }
                        m_DragObjects.push_back(actor);
                    } break;
                    case IConverter::ContentTexture: {
                        Actor *actor    = Engine::objectCreate<Actor>(findFreeObjectName(info.baseName().toStdString(), m_pMap));
                        SpriteMesh *sprite  = actor->addComponent<SpriteMesh>();
                        if(sprite) {
                            sprite->setMaterial(Engine::loadResource<Material>( DEFAULTSPRITE ));
                            sprite->setTexture(Engine::loadResource<Texture>( qPrintable(str) ));
                        }
                        m_DragObjects.push_back(actor);
                    } break;
                    default: break;
                }
            }
        }

    }
    for(Object *o : m_DragObjects) {
        Actor *a    = static_cast<Actor *>(o);
        a->setPosition(Vector3()); // \todo set drag position
        a->setParent(m_pMap);
    }
    if(!m_DragObjects.empty() || !m_DragMap.isEmpty()) {
        return;
    }

    event->ignore();
}

void ObjectCtrl::onDragLeave(QDragLeaveEvent * /*event*/) {
    for(Object *o : m_DragObjects) {
        delete o;
    }
    m_DragObjects.clear();
}

void ObjectCtrl::onInputEvent(QInputEvent *pe) {
    CameraCtrl::onInputEvent(pe);
    switch(pe->type()) {
        case QEvent::KeyPress: {
            QKeyEvent *e    = static_cast<QKeyEvent *>(pe);
            switch(e->key()) {
                case Qt::Key_Delete: {
                    deleteSelected();
                } break;
                case Qt::Key_Control: {
                    mAdditive   = true;
                } break;
                case Qt::Key_Shift: {
                    mCopy       = true;
                } break;
                default: break;
            }
        } break;
        case QEvent::KeyRelease: {
            QKeyEvent *e    = static_cast<QKeyEvent *>(pe);
            switch(e->key()) {
                case Qt::Key_Control: {
                    mAdditive   = false;
                } break;
                case Qt::Key_Shift: {
                    mCopy       = false;
                } break;
                default: break;
            }
        } break;
        case QEvent::MouseButtonPress: {
            QMouseEvent *e  = static_cast<QMouseEvent *>(pe);
            if(e->buttons() & Qt::LeftButton) {
                if(Handles::s_Axes) {
                    mAxes   = Handles::s_Axes;
                }
                if(mDrag) {
                    for(auto it : m_Selected) {
                        it.second.object->setPosition(it.second.position);
                        it.second.object->setEuler(it.second.euler);
                        it.second.object->setScale(it.second.scale);
                    }
                    if(m_pPropertyState) {
                        delete m_pPropertyState;
                        m_pPropertyState    = nullptr;
                    }
                    setDrag(false);
                }
            }
        } break;
        case QEvent::MouseButtonRelease: {
            QMouseEvent *e  = static_cast<QMouseEvent *>(pe);
            if(e->button() == Qt::LeftButton) {
                if(mDrag) {
                   UndoManager::instance()->push(m_pPropertyState);
                   m_pPropertyState = nullptr;
                } else {
                    selectActor( m_ObjectsList );

                    if(m_pPropertyState) {
                        delete m_pPropertyState;
                        m_pPropertyState    = nullptr;
                    }
                }
                setDrag(false);
            }
        } break;
        case QEvent::MouseMove: {
            QMouseEvent *e  = static_cast<QMouseEvent *>(pe);
            mMousePosition  = Vector2(e->pos().x(), e->pos().y());
            if(e->buttons() & Qt::LeftButton && !mDrag) {
                setDrag(Handles::s_Axes);
            }

            switch(mMode) {
                case MODE_TRANSLATE: {
                    if(mDrag) {
                        Vector3 delta = mWorld - mSaved;
                        if(mMoveGrid > 0.0f) {
                            for(uint32_t i = 0; i < 3; i++) {
                                delta[i]    = mMoveGrid * int(delta[i] / mMoveGrid);
                            }
                        }
                        for(const auto &it : m_Selected) {
                            Vector3 dt  = delta;
                            Actor *a    = dynamic_cast<Actor *>(it.second.object->parent());
                            if(a) {
                                Vector3 scale   = a->worldScale();
                                dt.x   /= scale.x;
                                dt.y   /= scale.y;
                                dt.z   /= scale.z;
                            }
                            it.second.object->setPosition(it.second.position + dt);
                        }
                        emit objectsUpdated();
                        emit objectsChanged(selected(), "Position");
                    } else {
                        Handles::s_Axes = mAxes;
                    }
                } break;
                case MODE_ROTATE: {
                    if(mDrag) {
                        Vector3 delta = mWorld - mSaved;
                        float angle   = (delta.x + delta.y + delta.z) * 0.5;
                        if(mAngleGrid > 0) {
                            angle   = mAngleGrid * int(angle / mAngleGrid);
                        }
                        for(const auto &it : m_Selected) {
                            Vector3 t       = Vector3(mPosition - it.second.position);
                            Quaternion q    = it.second.object->rotation();
                            Vector3 euler   = it.second.euler;
                            switch(Handles::s_Axes) {
                                case Handles::AXIS_X: {
                                    q       = q * Quaternion(Vector3(1.0f, 0.0f, 0.0f), angle);
                                    euler  += Vector3(angle, 0.0f, 0.0f);
                                } break;
                                case Handles::AXIS_Y: {
                                    q       = q * Quaternion(Vector3(0.0f, 1.0f, 0.0f), angle);
                                    euler  += Vector3(0.0f, angle, 0.0f);
                                } break;
                                case Handles::AXIS_Z: {
                                    q       = q * Quaternion(Vector3(0.0f, 0.0f, 1.0f), angle);
                                    euler  += Vector3(0.0f, 0.0f, angle);
                                } break;
                                default: {
                                    Vector3 axis  = m_pActiveCamera->actor().position() - mPosition;
                                    axis.normalize();
                                    q       = q * Quaternion(axis, angle);
                                    euler  += axis * angle;
                                } break;
                            }
                            it.second.object->setPosition(mPosition - q * t);
                            it.second.object->setEuler(euler);
                        }
                        emit objectsUpdated();
                        emit objectsChanged(selected(), "Rotation");
                    } else {
                        Handles::s_Axes = mAxes;
                    }
                } break;
                case MODE_SCALE: {
                    if(mDrag) {
                        Vector3 delta = (mWorld - mSaved);
                        float scale = (delta.x + delta.y + delta.z) * 0.01;
                        if(mScaleGrid > 0) {
                            scale = mScaleGrid * int(scale / mScaleGrid);
                        }
                        for(const auto &it : m_Selected) {
                            Vector3 s;
                            if(Handles::s_Axes & Handles::AXIS_X) {
                                s   += Vector3(scale, 0, 0);
                            }
                            if(Handles::s_Axes & Handles::AXIS_Y) {
                                s   += Vector3(0, scale, 0);
                            }
                            if(Handles::s_Axes & Handles::AXIS_Z) {
                                s   += Vector3(0, 0, scale);
                            }
                            it.second.object->setScale(it.second.scale + s);
                        }
                        emit objectsUpdated();
                        emit objectsChanged(selected(), "Scale");
                    } else {
                        Handles::s_Axes = Handles::AXIS_X | Handles::AXIS_Y | Handles::AXIS_Z;
                    }
                } break;
                default: break;
            }

            if(!m_ObjectsList.empty()) {
                m_pView->setCursor(QCursor(Qt::CrossCursor));
            } else {
                m_pView->unsetCursor();
            }
        } break;
        default: break;
    }
}

Object *ObjectCtrl::findObject(uint32_t id, Object *parent) {
    if(!parent) {
        parent  = m_pMap;
    }
    if(id && parent) {
        if(parent->uuid() == id) {
            return parent;
        }
        for(const auto &it : parent->getChildren()) {
            Object *object = it;
            if(object->uuid() != id) {
                object = findObject(id, object);
                if(!object) {
                    continue;
                }
            }
            return object;
        }
    }
    return nullptr;
}

void ObjectCtrl::resize(uint32_t width, uint32_t height) {
    m_Screen = Vector2(width, height);
}
