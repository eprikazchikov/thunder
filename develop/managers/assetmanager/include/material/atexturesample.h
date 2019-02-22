#ifndef TEXTURESAMPLE_H
#define TEXTURESAMPLE_H

#include "../shaderbuilder.h"

#include "assetmanager.h"

#define UV      "UV"

class TextureFunction : public ShaderFunction {
    Q_OBJECT
    Q_CLASSINFO("Group", "Texture")

public:
    static constexpr const char *R = "R";
    static constexpr const char *G = "G";
    static constexpr const char *B = "B";
    static constexpr const char *A = "A";

    Q_INVOKABLE TextureFunction() { }

    virtual AbstractSchemeModel::Node  *createNode  (ShaderBuilder *model, const QString &path) {
        AbstractSchemeModel::Node *result   = ShaderFunction::createNode(model, path);
        {
            AbstractSchemeModel::Item *out      = new AbstractSchemeModel::Item;
            out->name   = UV;
            out->out    = false;
            out->pos    = 0;
            out->type   = QMetaType::QVector2D;
            result->list.push_back(out);
        }
        {
            AbstractSchemeModel::Item *out      = new AbstractSchemeModel::Item;
            out->name   = "";
            out->out    = true;
            out->pos    = 0;
            out->type   = QMetaType::QVector4D;
            result->list.push_back(out);
        }
        {
            AbstractSchemeModel::Item *out      = new AbstractSchemeModel::Item;
            out->name   = R;
            out->out    = true;
            out->pos    = 1;
            out->type   = QMetaType::Double;
            result->list.push_back(out);
        }
        {
            AbstractSchemeModel::Item *out      = new AbstractSchemeModel::Item;
            out->name   = G;
            out->out    = true;
            out->pos    = 2;
            out->type   = QMetaType::Double;
            result->list.push_back(out);
        }
        {
            AbstractSchemeModel::Item *out      = new AbstractSchemeModel::Item;
            out->name   = B;
            out->out    = true;
            out->pos    = 3;
            out->type   = QMetaType::Double;
            result->list.push_back(out);
        }
        {
            AbstractSchemeModel::Item *out      = new AbstractSchemeModel::Item;
            out->name   = A;
            out->out    = true;
            out->pos    = 4;
            out->type   = QMetaType::Double;
            result->list.push_back(out);
        }

        return result;
    }

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        if(m_Position == -1) {
            QString uv  = "_uv0";
            const AbstractSchemeModel::Link *l  = m_pModel->findLink(m_pNode, UV);
            if(l) {
                ShaderFunction *node    = static_cast<ShaderFunction *>(l->sender->ptr);
                if(node) {
                    uint8_t type;
                    int32_t index   = node->build(value, *l, depth, type);
                    uv  = convert("local" + QString::number(index), type, QMetaType::QVector2D);
                }
            }
            uv      = QString("vec2(%1, %2) + %3 * vec2(%4, %5)").arg(m_Sub.x).arg(m_Sub.y).arg(uv).arg(m_Sub.z).arg(m_Sub.w);
            value  += QString("\tvec4 lt%1 = texture(uni.%2, %3);\n").arg(depth).arg(m_Name).arg(uv);

            if(link.sitem->name == "") {
                size    = QMetaType::QVector4D;
                value  += QString("\tvec4 local%1 = lt%1;\n").arg(depth);
            } else {
                size    = QMetaType::Double;

                QString channel = "x";
                if(link.sitem->name == G) {
                    channel = "y";
                } else if(link.sitem->name == B) {
                    channel = "z";
                } else if(link.sitem->name == A) {
                    channel = "w";
                }
                value  += QString("\tfloat local%1 = lt%1.%2;\n").arg(depth).arg(channel);
            }
        }
        return ShaderFunction::build(value, link, depth, size);
    }
protected:
    QString     m_Name;

    Vector4     m_Sub;

};

class TextureSample : public TextureFunction {
    Q_OBJECT
    Q_CLASSINFO("Group", "Texture")

    Q_PROPERTY(Template Texture READ texture WRITE setTexture NOTIFY updated DESIGNABLE true USER true)

public:
    Q_INVOKABLE TextureSample() {
        m_Path  = Template("", MetaType::type<Texture *>());
    }

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        int result  = m_pModel->setTexture(m_Path.path, m_Sub, false);

        if(result < 0) {
            m_pModel->reportError(this, "Missing texture");
            return false;
        }
        m_Name  = QString("texture%1").arg(result);
        return TextureFunction::build(value, link, depth, size);
    }

    Template    texture     () const {
        return m_Path;
    }

    void        setTexture  (const Template &path) {
        m_Path.path = path.path;
        emit updated();
    }

protected:
    Template    m_Path;

};

class RenderTargetSample : public TextureFunction {
    Q_OBJECT
    Q_CLASSINFO("Group", "Texture")

    Q_PROPERTY(QString Target_Name READ targetName WRITE setTargetName NOTIFY updated DESIGNABLE true USER true)

public:
    Q_INVOKABLE RenderTargetSample() { }

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        m_pModel->setTexture(m_Name, m_Sub, ShaderBuilder::Target);

        return TextureFunction::build(value, link, depth, size);
    }

    QString     targetName      () const {
        return m_Name;
    }

    void        setTargetName   (const QString &name) {
        m_Name  = name;
        emit updated();
    }
};

class TextureSampleCube : public TextureSample {
    Q_OBJECT
    Q_CLASSINFO("Group", "Texture")

public:
    Q_INVOKABLE TextureSampleCube() {}

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        if(m_Position == -1) {
            Vector4 sub;
            int result  = m_pModel->setTexture(m_Path.path, sub, ShaderBuilder::Cube);

            if(result > -1) {
                QString uv  = "p.uv";
                const AbstractSchemeModel::Link *l  = m_pModel->findLink(m_pNode, UV);
                if(l) {
                    TextureSample *node = static_cast<TextureSample *>(l->sender->ptr);
                    if(node) {
                        uint8_t type;
                        uint32_t index  = node->build(value, *l, depth, type);
                        uv  = convert("local" + QString::number(index), type, QMetaType::QVector3D);
                    }
                }

                uv      = "vec3(" + uv + ", 1.0)";
                value  += QString("\tvec4 lt%1 = texture(uni.texture%2, %3);\n").arg(depth).arg(result).arg(uv);
                if(link.sitem->name == "") {
                    size    = MetaType::VECTOR4;
                    value  += QString("\tvec4 local%1 = lt%1;\n").arg(depth);
                } else {
                    size    = MetaType::FLOAT;
                    QString channel = "x";
                    if(link.sitem->name == G) {
                        channel = "y";
                    } else if(link.sitem->name == B) {
                        channel = "z";
                    } else if(link.sitem->name == A) {
                        channel = "w";
                    }
                    value  += QString("\tfloat local%1 = lt%1.%2;\n").arg(depth).arg(channel);
                }
            } else {
                m_pModel->reportError(this, "Missing texture");
                return -1;
            }
        }
        return ShaderFunction::build(value, link, depth, size);
    }

};

#endif // TEXTURESAMPLE_H
