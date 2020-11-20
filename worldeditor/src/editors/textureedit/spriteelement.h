#ifndef SPRITEELEMENT_H
#define SPRITEELEMENT_H

#include <QWidget>

namespace Ui {
    class SpriteElement;
}

class TextureImportSettings;

class SpriteElement : public QWidget {
    Q_OBJECT

public:
    explicit SpriteElement(QWidget *parent = nullptr);
    ~SpriteElement();

    void setSettings(TextureImportSettings *settings);

public slots:
    void onSelectionChanged(const QString &key);
    void onElementUpdated();
    void onElementChanged();

private:
    void paintEvent(QPaintEvent *pe) Q_DECL_OVERRIDE;

private:
    Ui::SpriteElement *ui;

    TextureImportSettings *m_pSettings;

    QString m_Key;
};

#endif // SPRITEELEMENT_H
