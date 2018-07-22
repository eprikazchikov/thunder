#include "scenecomposer.h"
#include "ui_scenecomposer.h"

#include <QSettings>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include <QDateTime>
#include <QVariant>
#include <QWidgetAction>

#include <json.h>
#include <bson.h>

#if __linux__
#include <cstring>
#endif

// Engine
#include <module.h>
#include <timer.h>
#include <components/chunk.h>
#include <components/actor.h>
#include <components/spritemesh.h>
#include <components/staticmesh.h>
#include <components/camera.h>

#include <analytics/profiler.h>

// Misc
#include "controllers/objectctrl.h"
#include "graph/sceneview.h"

#include "managers/undomanager/undomanager.h"
#include "managers/pluginmanager/plugindialog.h"
#include "managers/configmanager/configdialog.h"

#include "assetmanager.h"
#include "projectmanager.h"

#include "aboutdialog.h"

// System
#include "common.h"
#include "qlog.h"

// Editors
#include "editors/propertyedit/nextobject.h"
#include "editors/contentbrowser/contentlist.h"
#include "editors/componentbrowser/componentmodel.h"
#include "editors/textureedit/textureedit.h"
#include "editors/materialedit/materialedit.h"
#include "editors/meshedit/meshedit.h"

#include "managers/asseteditormanager/importqueue.h"
#include "managers/asseteditormanager/iconrender.h"

#define FPS         "FPS"
#define VERTICES    "Vertices"
#define POLYGONS    "Polygons"
#define DRAWCALLS   "Draw Calls"

Q_DECLARE_METATYPE(Object *)
Q_DECLARE_METATYPE(Actor *)

const QString gRecent("Recent");

SceneComposer::SceneComposer(Engine *engine, QWidget *parent) :
        QMainWindow(parent),
        m_pImportQueue(new ImportQueue()),
        ui(new Ui::SceneComposer),
        m_pProperties(nullptr),
        m_pMap(nullptr) {

    qRegisterMetaType<Vector2>  ("Vector2");
    qRegisterMetaType<Vector3>  ("Vector3");

    qRegisterMetaType<uint8_t>  ("uint8_t");
    qRegisterMetaType<uint32_t> ("uint32_t");

    resetModified();

    ui->setupUi(this);

    m_pBuilder  = new QProcess(this);
    connect( m_pBuilder, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()) );
    connect( m_pBuilder, SIGNAL(readyReadStandardError()), this, SLOT(readError()) );
    connect( m_pBuilder, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onFinished(int,QProcess::ExitStatus)) );

    m_pEngine   = engine;

    QLog *log   = new QLog();
    connect(log, SIGNAL(postRecord(uint8_t, const QString &)), ui->consoleOutput, SLOT(onLogRecord(uint8_t, const QString &)));
    Log::overrideHandler(log);
    Log::setLogLevel(Log::DBG);

    cmToolbars      = new QMenu(this);

    m_pPluginDlg    = new PluginDialog(m_pEngine, this);
    connect(m_pPluginDlg, SIGNAL(updated()), ComponentModel::instance(), SLOT(update()));

    ObjectCtrl *ctl = new ObjectCtrl(ui->viewport);
    connect(ui->viewport, SIGNAL(drop(QDropEvent*)), ctl, SLOT(onDrop()));
    connect(ui->viewport, SIGNAL(dragEnter(QDragEnterEvent *)), ctl, SLOT(onDragEnter(QDragEnterEvent *)));
    connect(ui->viewport, SIGNAL(dragLeave(QDragLeaveEvent *)), ctl, SLOT(onDragLeave(QDragLeaveEvent *)));

    ui->viewport->setController(ctl);
    ui->viewport->setScene(Engine::objectCreate<Scene>("Scene"));

    ui->preview->setController(new IController());
    ui->preview->setScene(ui->viewport->scene());
    ui->preview->setWindowTitle("Preview");

    ui->viewportWidget->setWindowTitle("Viewport");
    ui->propertyWidget->setWindowTitle("Properties");
    ui->projectWidget->setWindowTitle("Project Settings");

    ui->commitButton->setProperty("green", true);
    ui->componentButton->setProperty("blue", true);
    ui->moveButton->setProperty("blue", true);
    ui->rotateButton->setProperty("blue", true);
    ui->scaleButton->setProperty("blue", true);

    ComponentBrowser *comp  = new ComponentBrowser(this);
    QMenu *menu = new QMenu(ui->componentButton);
    QWidgetAction *action   = new QWidgetAction(menu);
    action->setDefaultWidget(comp);
    menu->addAction(action);
    ui->componentButton->setMenu(menu);
    connect(comp, SIGNAL(componentSelected(QString)), ctl, SLOT(onComponentSelected(QString)));
    connect(comp, SIGNAL(componentSelected(QString)), menu, SLOT(hide()));

    comp->setGroups(QStringList("Components"));
    ui->components->setGroups(QStringList() << "Scene" << "Components");

    connect(ui->commitButton, SIGNAL(clicked(bool)), ProjectManager::instance(), SLOT(saveSettings()));

    connect(ui->viewport, SIGNAL(inited()), this, SLOT(onGLInit()));
    startTimer(16);

    ui->centralwidget->addToolWindow(ui->viewportWidget, QToolWindowManager::EmptySpaceArea);
    ui->centralwidget->addToolWindow(ui->preview, QToolWindowManager::EmptySpaceArea);
    ui->centralwidget->addToolWindow(ui->contentBrowser, QToolWindowManager::ReferenceBottomOf, ui->centralwidget->areaFor(ui->viewportWidget));
    ui->centralwidget->addToolWindow(ui->hierarchy, QToolWindowManager::ReferenceRightOf, ui->centralwidget->areaFor(ui->viewportWidget));
    ui->centralwidget->addToolWindow(ui->propertyWidget, QToolWindowManager::ReferenceBottomOf, ui->centralwidget->areaFor(ui->hierarchy));
    ui->centralwidget->addToolWindow(ui->components, QToolWindowManager::ReferenceLeftOf, ui->centralwidget->areaFor(ui->viewportWidget));
    ui->centralwidget->addToolWindow(ui->consoleOutput, QToolWindowManager::ReferenceRightOf, ui->centralwidget->areaFor(ui->contentBrowser));
    ui->centralwidget->addToolWindow(ui->projectWidget, QToolWindowManager::NoArea);

    ui->actionAbout->setText(tr("About %1...").arg(EDITOR_NAME));
    foreach(QWidget *it, ui->centralwidget->toolWindows()) {
        QAction *action = new QAction(it->windowTitle(), ui->menuWindow);
        ui->menuWindow->insertAction(ui->actionSave_Layout, action);
        action->setObjectName(it->windowTitle());
        action->setData(QVariant::fromValue(it));
        action->setCheckable(true);
        action->setChecked(true);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(onToolWindowActionToggled(bool)));
    }
    ui->menuWindow->insertSeparator(ui->actionSave_Layout);

    connect(ui->centralwidget, SIGNAL(toolWindowVisibilityChanged(QWidget *, bool)), this, SLOT(onToolWindowVisibilityChanged(QWidget *, bool)));

    connect(ctl, SIGNAL(mapUpdated()), ui->hierarchy, SLOT(onHierarchyUpdated()));
    connect(ctl, SIGNAL(objectsSelected(Object::ObjectList)), this, SLOT(onObjectSelected(Object::ObjectList)));
    connect(ctl, SIGNAL(objectsSelected(Object::ObjectList)), ui->hierarchy, SLOT(onSelected(Object::ObjectList)));
    connect(ctl, SIGNAL(mapUpdated()), this, SLOT(onModified()));
    connect(ctl, SIGNAL(objectsUpdated()), this, SLOT(onModified()));
    connect(ctl, SIGNAL(loadMap(QString)), this, SLOT(on_action_Open_triggered(QString)));
    connect(ui->hierarchy, SIGNAL(selected(Object::ObjectList)), ctl, SLOT(onSelectActor(Object::ObjectList)));
    connect(ui->hierarchy, SIGNAL(removed(Object::ObjectList)), ctl, SLOT(onRemoveActor(Object::ObjectList)));
    connect(ui->hierarchy, SIGNAL(parented(Object::ObjectList, Object::ObjectList)), ctl, SLOT(onParentActor(Object::ObjectList,Object::ObjectList)));
    connect(ui->hierarchy, SIGNAL(focused(Object*)), ctl, SLOT(onFocusActor(Object*)));
    connect(ui->orthoButton,SIGNAL(toggled(bool)), ctl, SLOT(onOrthographic(bool)));
    connect(ui->moveButton,     SIGNAL(clicked()), ctl, SLOT(onMoveActor()));
    connect(ui->rotateButton,   SIGNAL(clicked()), ctl, SLOT(onRotateActor()));
    connect(ui->scaleButton,    SIGNAL(clicked()), ctl, SLOT(onScaleActor()));

    ui->scaleButton->click();

    connect(UndoManager::instance(), SIGNAL(updated()), this, SLOT(onUndoRedoUpdated()));

    connect(ui->hierarchy, SIGNAL(updated()), ui->propertyView, SLOT(onUpdated()));
    connect(ui->hierarchy, SIGNAL(updated()), this, SLOT(onModified()));

    connect(m_pImportQueue, SIGNAL(rendered(QString)), ContentList::instance(), SLOT(onRendered(QString)));

    AssetManager *asset = AssetManager::instance();
    asset->addEditor(IConverter::ContentTexture, new TextureEdit(m_pEngine));
    asset->addEditor(IConverter::ContentMaterial, new MaterialEdit(m_pEngine));
    asset->addEditor(IConverter::ContentMesh, new MeshEdit(m_pEngine));

    ui->projectSettings->setObject(ProjectManager::instance());

    on_actionEditor_Mode_triggered();
    on_actionResore_Layout_triggered();

    on_action_New_triggered();
}

SceneComposer::~SceneComposer() {
    AssetManager::destroy();

    delete m_pProperties;

    delete cmToolbars;

    delete ui;
}

void SceneComposer::timerEvent(QTimerEvent *) {
    Timer::update();
    ui->viewport->update();
    ui->preview->update();
}

void SceneComposer::onObjectSelected(Object::ObjectList objects) {
    if(m_pProperties) {
        delete m_pProperties;
        m_pProperties   = 0;
    }
    if(!objects.empty()) {
        ui->viewport->makeCurrent();
        /// \todo Don't reload mesh and materials for each repick
        m_pProperties   = new NextObject(*objects.begin(), static_cast<ObjectCtrl *>(ui->viewport->controller()), this);
        connect(static_cast<CameraCtrl *>(ui->viewport->controller()), SIGNAL(objectsUpdated()), m_pProperties, SLOT(onUpdated()));
        connect(m_pPluginDlg, SIGNAL(pluginReloaded()), m_pProperties, SLOT(onUpdated()));
        connect(m_pProperties, SIGNAL(updated()), ui->propertyView, SLOT(onUpdated()));
        connect(m_pProperties, SIGNAL(updated()), this, SLOT(onModified()));
    }
    ui->propertyView->setObject(m_pProperties);
}

void SceneComposer::onGLInit() {
    m_pImportQueue->init(new IconRender(m_pEngine, ui->viewport->context()));

    ComponentModel::instance()->init(m_pEngine);
    ContentList::instance()->init(m_pEngine);

    QSettings settings(COMPANY_NAME, EDITOR_NAME);
    QVariant map    = settings.value(ProjectManager::instance()->projectId());
    if(map.isValid()) {
        VariantList list    =  Json::load(map.toString().toStdString()).toList();
        auto it = list.begin();
        on_action_Open_triggered(it->toString().c_str());
        it++;
        Camera *camera  = ui->viewport->controller()->activeCamera();
        if(camera) {
            Actor &actor    = camera->actor();
            actor.setPosition(it->toVector3());
            it++;
            actor.setEuler(it->toVector3());
            it++;
            ui->orthoButton->setChecked(it->toBool());
            it++;
            camera->setFocal(it->toFloat());
            it++;
            camera->setOrthoWidth(it->toFloat());
            it++;
        }

    }
}

void SceneComposer::updateTitle() {
    if(!mPath.isEmpty()) {
        setWindowTitle(mPath + " - " + QString(EDITOR_NAME));
    } else {
        setWindowTitle(EDITOR_NAME);
    }
}

void SceneComposer::closeEvent(QCloseEvent *event) {
    QMainWindow::closeEvent(event);

    if(!checkSave()) {
        event->ignore();
        return;
    }

    QString str = ProjectManager::instance()->projectId();
    if(!str.isEmpty() && !mPath.isEmpty()) {
        VariantList params;
        params.push_back(qPrintable(mPath));
        Camera *camera  = ui->viewport->controller()->activeCamera();
        if(camera) {
            Actor &actor    = camera->actor();
            params.push_back(actor.position());
            params.push_back(actor.euler());
            params.push_back(ui->orthoButton->isChecked());
            params.push_back(camera->focal());
            params.push_back(camera->orthoWidth());
        }

        QSettings settings(COMPANY_NAME, EDITOR_NAME);
        settings.setValue(str, QString::fromStdString(Json::save(params)));
    }

    QApplication::quit();
}

bool SceneComposer::checkSave() {
    if(isModified()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText("The map has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);

        int result  = msgBox.exec();
        if(result == QMessageBox::Cancel) {
            return false;
        }
        if(result == QMessageBox::Yes) {
            on_actionSave_triggered();
        }
    }
    return true;
}

void SceneComposer::on_action_New_triggered() {
    checkSave();

    delete m_pMap;

    m_pMap  = Engine::objectCreate<Chunk>("Chunk", ui->viewport->scene());
    ui->hierarchy->setObject(m_pMap);

    ObjectCtrl *ctrl    = static_cast<ObjectCtrl *>(ui->viewport->controller());

    ctrl->clear();
    ctrl->setMap(m_pMap);

    UndoManager::instance()->clear();
    onUndoRedoUpdated();

    mPath.clear();
    updateTitle();
}

void SceneComposer::on_action_Open_triggered(const QString &arg) {
    checkSave();
    if(arg.isEmpty()) {
        QString dir = ProjectManager::instance()->contentPath();
        mPath       = QFileDialog::getOpenFileName(this,
                                                   tr("Open Map"),
                                                   dir + "/maps",
                                                   tr("Maps (*.map)") );
    } else {
        mPath   = arg;
    }

    if( !mPath.isEmpty() ) {
        QFile loadFile(mPath);
        if (!loadFile.open(QIODevice::ReadOnly)) {
            qWarning("Couldn't open file.");
            return;
        }

        QByteArray array    = loadFile.readAll();
        string data;
        data.resize(array.size());
        memcpy(&data[0], array.data(), array.size());
        Variant var = Json::load(data);
        Object *map = Engine::toObject(var);
        if(map) {
            updateTitle();

            map->setParent(ui->viewport->scene());
            delete m_pMap;
            m_pMap  = map;
            ui->hierarchy->setObject(m_pMap);
            ObjectCtrl * ctrl   = static_cast<ObjectCtrl *>(ui->viewport->controller());

            ctrl->clear();
            ctrl->setMap(m_pMap);

            UndoManager::instance()->clear();
            onUndoRedoUpdated();
        }
    }
}

void SceneComposer::on_actionSave_triggered() {
    if(m_pMap) {
        if( mPath.length() > 0 ) {
            QDir dir    = QDir(QDir::currentPath());

            QFile file(dir.relativeFilePath(mPath));
            if(file.open(QIODevice::WriteOnly)) {
                string data = Json::save(Engine::toVariant(m_pMap), 0);
                file.write((const char *)&data[0], data.size());
                file.close();
            }
            resetModified();
        } else {
            on_actionSave_As_triggered();
        }
    }
}

void SceneComposer::on_actionSave_As_triggered() {
    QString dir = ProjectManager::instance()->contentPath();
    mPath       = QFileDialog::getSaveFileName(this,
                                               tr("Save Map"),
                                               dir + "/maps",
                                               tr("Maps (*.map)") );
    if( mPath.length() > 0 ) {
        updateTitle();

        on_actionSave_triggered();
    }
}

void SceneComposer::on_actionPlugin_Manager_triggered() {
    m_pPluginDlg->exec();
}

void SceneComposer::on_actionEditor_Mode_triggered() {
    ui->actionEditor_Mode->setChecked(true);
    ui->actionGame_Mode->setChecked(false);

    ui->preview->stopGame();
    ui->centralwidget->activateToolWindow(ui->viewport);
    Object *map = Engine::toObject(Bson::load(m_Back));
    if(map) {
        map->setParent(ui->viewport->scene());
        delete m_pMap;
        m_pMap  = map;
        ui->hierarchy->setObject(m_pMap);
        ObjectCtrl * ctrl   = static_cast<ObjectCtrl *>(ui->viewport->controller());

        ctrl->clear();
        ctrl->setMap(m_pMap);
    }
}

void SceneComposer::on_actionGame_Mode_triggered() {
    ui->actionGame_Mode->setChecked(true);
    ui->actionEditor_Mode->setChecked(false);

    if(!ui->preview->isGame()) {
        m_Back  = Bson::save(Engine::toVariant(m_pMap));

        ui->preview->startGame();
        ui->centralwidget->activateToolWindow(ui->preview);
    }
}

void SceneComposer::on_actionTake_Screenshot_triggered() {
    QImage result   = ui->viewport->grabFramebuffer();
    result.save("SceneComposer-" + QDateTime::currentDateTime().toString("ddMMyy-HHmmss") + ".png");
}

void SceneComposer::onUndoRedoUpdated() {
    UndoManager *mgr    = UndoManager::instance();
    QString Undo    = mgr->undoTop();
    QString Redo    = mgr->redoTop();

    ui->actionUndo->setDisabled( Undo.isEmpty() );
    ui->actionRedo->setDisabled( Redo.isEmpty() );

    ui->actionUndo->setText(tr("Undo ") + Undo);
    ui->actionRedo->setText(tr("Redo ") + Redo);
}

void SceneComposer::on_actionUndo_triggered() {
    UndoManager::instance()->undo();
}

void SceneComposer::on_actionRedo_triggered() {
    UndoManager::instance()->redo();
}

void SceneComposer::onToolWindowActionToggled(bool state) {
    QWidget *toolWindow = static_cast<QAction*>(sender())->data().value<QWidget *>();
    ui->centralwidget->moveToolWindow(toolWindow, state ?
                                              QToolWindowManager::NewFloatingArea :
                                              QToolWindowManager::NoArea);
}

void SceneComposer::onToolWindowVisibilityChanged(QWidget *toolWindow, bool visible) {
    QAction *action = ui->menuWindow->findChild<QAction *>(toolWindow->windowTitle());
    if(action) {
        action->blockSignals(true);
        action->setChecked(visible);
        action->blockSignals(false);
    }
}

void SceneComposer::on_actionSave_Layout_triggered() {
    QSettings settings(COMPANY_NAME, EDITOR_NAME);
    settings.setValue("main.geometry", saveGeometry());
    settings.setValue("main.windows", ui->centralwidget->saveState());
}

void SceneComposer::on_actionResore_Layout_triggered() {
    QSettings settings(COMPANY_NAME, EDITOR_NAME);
    restoreGeometry (settings.value("main.geometry").toByteArray());
    ui->centralwidget->restoreState(settings.value("main.windows"));
}

void SceneComposer::onModified() {
    mModified   = true;
}

void SceneComposer::on_actionBuild_Project_triggered() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Target Directory"),
                                                 "",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    if(!dir.isEmpty()) {
        ProjectManager *mgr = ProjectManager::instance();

        QStringList args;
        args << "-s" << mgr->projectPath() << "-t" << dir;

        m_pBuilder->start("Builder", args);
        if(!m_pBuilder->waitForStarted()) {
            Log(Log::ERR) << qPrintable(m_pBuilder->errorString());
        }
    }
}

void SceneComposer::readOutput() {
    QProcess *p = dynamic_cast<QProcess *>( sender() );
    if(p) {
        parseLogs(p->readAllStandardOutput());
    }
}

void SceneComposer::readError() {
    QProcess *p = dynamic_cast<QProcess *>( sender() );
    if(p) {
        parseLogs(p->readAllStandardError());
    }
}

void SceneComposer::onFinished(int exitCode, QProcess::ExitStatus) {
    if(exitCode == 0) {
        Log(Log::INF) << "Build Finished";
    } else {
        Log(Log::ERR) << "Build Failed";
    }
}

void SceneComposer::parseLogs(const QString &log) {
    QStringList list = log.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

    foreach(QString it, list) {
        if(it.contains(" error ")) {
            Log(Log::ERR) << qPrintable(it);
        } else if(it.contains(" warning ")) {
            Log(Log::WRN) << qPrintable(it);
        } else {
            Log(Log::INF) << qPrintable(it);
        }
    }
}

void SceneComposer::on_actionOptions_triggered() {
    ConfigDialog dlg;
    dlg.exec();
}

void SceneComposer::on_actionAbout_triggered() {
    AboutDialog dlg;
    dlg.exec();
}
