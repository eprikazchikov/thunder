#include "textureedit.h"
#include "ui_textureedit.h"

#include "textureconverter.h"

#include "editors/propertyedit/nextobject.h"
#include "controllers/objectctrl.h"
#include "graph/sceneview.h"

#include "components/actor.h"
#include "components/transform.h"
#include "components/spriterender.h"
#include "components/camera.h"

#include "resources/texture.h"
#include "resources/material.h"

#include "projectmanager.h"

#include "spritecontroller.h"

#include "spriteelement.h"

TextureEdit::TextureEdit(DocumentModel *document) :
        QWidget(nullptr),
        ui(new Ui::TextureEdit),
        m_pRender(nullptr),
        m_pSettings(nullptr),
        m_pConverter(new TextureConverter),
        m_pDocument(document),
        m_Details(new SpriteElement(this)) {

    ui->setupUi(this);

    ui->preview->setScene(Engine::objectCreate<Scene>("Scene"));

    connect(ui->preview, &Viewport::inited, this, &TextureEdit::onGLInit);
    startTimer(16);
}

TextureEdit::~TextureEdit() {
    delete ui;
}

void TextureEdit::timerEvent(QTimerEvent *) {
    ui->preview->repaint();
}

void TextureEdit::closeEvent(QCloseEvent *event) {
    if(!m_pDocument->checkSave(this)) {
        event->ignore();
        return;
    }
    QDir dir(ProjectManager::instance()->contentPath());
    m_pDocument->closeFile(dir.relativeFilePath(m_Path));
}

bool TextureEdit::isModified() const {
    return m_pSettings->isModified();
}

void TextureEdit::loadAsset(IConverterSettings *settings) {
    show();
    raise();

    if(m_pSettings) {
        disconnect(m_pSettings, &IConverterSettings::updated, this, &TextureEdit::onUpdateTemplate);
    }
    m_pSettings = dynamic_cast<TextureImportSettings *>(settings);

    m_Path = settings->source();

    Resource *resource = Engine::loadResource<Resource>(qPrintable(settings->destination()));
    Sprite *sprite = dynamic_cast<Sprite *>(resource);
    if(sprite) {
        m_pRender->setSprite(sprite);
    } else {
        Texture *texture = dynamic_cast<Texture *>(resource);
        if(texture) {
            m_pRender->setTexture(texture);
        }
    }

    SpriteController *ctrl = static_cast<SpriteController *>(ui->preview->controller());
    ctrl->setImportSettings(dynamic_cast<TextureImportSettings *>(m_pSettings));
    ctrl->setSize(m_pRender->texture()->width(),
                  m_pRender->texture()->height());

    m_Details->setSettings(m_pSettings);
    m_Details->raise();

    connect(m_pSettings, &IConverterSettings::updated, this, &TextureEdit::onUpdateTemplate);
}

QStringList TextureEdit::assetTypes() const {
    return {"Texture", "Sprite"};
}

void TextureEdit::onUpdateTemplate() {
    if(m_pSettings) {
        m_pConverter->convertTexture(m_pSettings, m_pRender->texture());
    }
}

void TextureEdit::onGLInit() {
    Scene *scene = ui->preview->scene();

    SpriteController *ctrl = new SpriteController(ui->preview);
    ctrl->blockRotations(true);
    ctrl->init(scene);

    connect(ctrl, &SpriteController::selectionChanged, m_Details, &SpriteElement::onSelectionChanged);

    ui->preview->setController(ctrl);

    Camera *camera = ui->preview->controller()->camera();
    if(camera) {
        camera->setOrthographic(true);
    }

    Actor *object = Engine::objectCreate<Actor>("Sprite", scene);
    object->transform()->setScale(Vector3(SCALE));
    m_pRender = static_cast<SpriteRender *>(object->addComponent("SpriteRender"));
    if(m_pRender) {
        m_pRender->setMaterial(Engine::loadResource<Material>(".embedded/DefaultSprite.mtl"));
    }
}

void TextureEdit::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    QRect r = m_Details->geometry();
    r.setX(20);
    r.setY(10);
    m_Details->setGeometry(r);
}
