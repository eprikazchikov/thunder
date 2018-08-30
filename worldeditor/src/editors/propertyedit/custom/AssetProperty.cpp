#include <QPainter>

#include "AssetProperty.h"

#include <engine.h>

#include "editors/contentbrowser/contentselect.h"

#include "assetmanager.h"


TemplateProperty::TemplateProperty(const QString &name, QObject *propertyObject, QObject *parent) :
        Property(name, propertyObject, parent) {

}

QVariant TemplateProperty::value(int role) const {
    QVariant data   = Property::value(role);
    if(role == Qt::EditRole) {
        return data.value<Template>().path;
    }
    return data;
}

void TemplateProperty::setValue(const QVariant &value) {
    if (value.type() == QVariant::String) {
        Template tmp(value.toString(), Property::value(Qt::EditRole).value<Template>().type);
        Property::setValue(QVariant::fromValue( tmp ));
    } else {
        Property::setValue(value);
    }
}

QWidget *TemplateProperty::createEditor(QWidget *parent, const QStyleOptionViewItem &) {
    ContentSelect *editor = new ContentSelect(parent);
    int type    = Property::value(Qt::EditRole).value<Template>().type;
    editor->setType( type );

    m_Editor    = editor;
    connect(m_Editor, SIGNAL(assetChanged(IConverterSettings *)), this, SLOT(onAssetChanged(IConverterSettings *)));
    return m_Editor;
}

bool TemplateProperty::setEditorData(QWidget *editor, const QVariant &data) {
    ContentSelect *e  = dynamic_cast<ContentSelect *>(editor);
    if(e) {
        e->blockSignals(true);
        e->setData(data.toString());
        e->blockSignals(false);
        return true;
    }
    return Property::setEditorData(editor, data);
}

QVariant TemplateProperty::editorData(QWidget *editor) {
    ContentSelect *e  = dynamic_cast<ContentSelect *>(editor);
    if(e) {
        return e->data();
    }
    return Property::editorData(editor);
}

QSize TemplateProperty::sizeHint(const QSize& size) const {
    return QSize(size.width(), 74);
}

void TemplateProperty::onAssetChanged(IConverterSettings *settings) {
    setValue(settings->destination());
}
