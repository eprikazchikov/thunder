#ifndef AMATHFUNCTIONS
#define AMATHFUNCTIONS

#include "../shaderbuilder.h"

#define MINV    "Min"
#define MAXV    "Max"

class MathFunction : public ShaderFunction {
    Q_OBJECT
    Q_CLASSINFO("Group", "Math")

public:
    static constexpr const char *A = "A";
    static constexpr const char *B = "B";

    AbstractSchemeModel::Node *createNode(ShaderBuilder *model, const QString &path) {
        AbstractSchemeModel::Node *result   = ShaderFunction::createNode(model, path);
        int i   = 0;
        foreach(QString it, m_Params) {
            AbstractSchemeModel::Item *out  = new AbstractSchemeModel::Item;
            out->name   = it;
            out->out    = false;
            out->pos    = i;
            out->type   = QMetaType::Void;
            result->list.push_back(out);
            i++;
        }
        {
            AbstractSchemeModel::Item *out  = new AbstractSchemeModel::Item;
            out->name   = "";
            out->out    = true;
            out->pos    = 0;
            out->type   = QMetaType::Void;
            result->list.push_back(out);
        }
        return result;
    }

    uint32_t compile(AbstractSchemeModel::Node *object, const QString &func, QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size, uint8_t expect = 0) {
        if(m_Position == -1) {
            QString args;

            const AbstractSchemeModel::Link *l;
            int i       = 0;
            foreach(QString it, m_Params) {
                l   = m_pModel->findLink(object, qPrintable(it));
                if(l) {
                    ShaderFunction *node    = static_cast<ShaderFunction *>(l->sender->ptr);
                    if(node) {
                        uint8_t type;
                        uint32_t index  = node->build(value, *l, depth, type);
                        if(i == 0 && !expect) {
                            expect  = type;
                        }
                        args    += convert(QString("local%1").arg(index), type, expect) + ((i == m_Params.size() - 1) ? "" : ", ");
                    }
                } else {
                    m_pModel->reportError(this, QString("Missing argument ") + it);
                    return -1;
                }
                i++;
            }

            if(!size) {
                size    = expect;
            }

            switch (size) {
                case QMetaType::QVector2D:  value.append("\tvec2");  break;
                case QMetaType::QVector3D:  value.append("\tvec3");  break;
                case QMetaType::QVector4D:  value.append("\tvec4");  break;
                default: value.append("\tfloat"); break;
            }
            value.append(QString(" local%1 = %2(%3);\n").arg(depth).arg(func).arg(args));
        }
        return ShaderFunction::build(value, link, depth, size);
    }
    QStringList     m_Params;
};

class DotProduct : public MathFunction {
    Q_OBJECT

public:
    Q_INVOKABLE DotProduct() { m_Params << A << B; }

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        size    = QMetaType::Double;
        return compile(m_pNode, "dot", value, link, depth, size);
    }
};

class CrossProduct : public MathFunction {
    Q_OBJECT

public:
    Q_INVOKABLE CrossProduct() { m_Params << A << B; }

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        size    = QMetaType::QVector3D;
        return compile(m_pNode, "cross", value, link, depth, size, size);
    }
};

class Clamp : public MathFunction {
    Q_OBJECT

public:
    Q_INVOKABLE Clamp() { m_Params << A << MINV << MAXV; }

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        return compile(m_pNode, "clamp", value, link, depth, size);
    }
};

class Mod : public MathFunction {
    Q_OBJECT

public:
    Q_INVOKABLE Mod() { m_Params << A << B; }

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        return compile(m_pNode, "mod", value, link, depth, size);
    }
};

class Abs : public MathFunction {
    Q_OBJECT

public:
    Q_INVOKABLE Abs() { m_Params << A; }

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        return compile(m_pNode, "abs", value, link, depth, size);
    }
};

class Floor : public MathFunction {
    Q_OBJECT

public:
    Q_INVOKABLE Floor() { m_Params << A; }

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        return compile(m_pNode, "floor", value, link, depth, size);
    }
};

class Ceil : public MathFunction {
    Q_OBJECT

public:
    Q_INVOKABLE Ceil() { m_Params << A; }

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        return compile(m_pNode, "ceil", value, link, depth, size);
    }
};

class Normalize : public MathFunction {
    Q_OBJECT

public:
    Q_INVOKABLE Normalize() { m_Params << A; }

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        return compile(m_pNode, "normalize", value, link, depth, size);
    }
};

class Sine : public MathFunction {
    Q_OBJECT

public:
    Q_INVOKABLE Sine() { m_Params << A; }

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        return compile(m_pNode, "sin", value, link, depth, size);
    }
};

class Cosine : public MathFunction {
    Q_OBJECT

public:
    Q_INVOKABLE Cosine() { m_Params << A; }

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        return compile(m_pNode, "cos", value, link, depth, size);
    }
};

class Tangent : public MathFunction {
    Q_OBJECT

public:
    Q_INVOKABLE Tangent() { m_Params << A; }

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        return compile(m_pNode, "tan", value, link, depth, size);
    }
};

class ArcSine : public MathFunction {
    Q_OBJECT

public:
    Q_INVOKABLE ArcSine() { m_Params << A; }

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        return compile(m_pNode, "asin", value, link, depth, size);
    }
};

class ArcCosine : public MathFunction {
    Q_OBJECT

public:
    Q_INVOKABLE ArcCosine() { m_Params << A; }

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        return compile(m_pNode, "acos", value, link, depth, size);
    }
};

class ArcTangent : public MathFunction {
    Q_OBJECT

public:
    Q_INVOKABLE ArcTangent() {m_Params << A << B; }

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        return compile(m_pNode, "atan", value, link, depth, size);
    }
};

#endif // AMATHFUNCTIONS
