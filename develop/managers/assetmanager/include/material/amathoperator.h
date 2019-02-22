#ifndef AMATHOPERATOR
#define AMATHOPERATOR

#include "../shaderbuilder.h"

class MathOperation : public ShaderFunction {
    Q_OBJECT
    Q_CLASSINFO("Group", "Operations")

public:
    static constexpr const char *A = "A";
    static constexpr const char *B = "B";
    static constexpr const char *OUT = "Out";

    AbstractSchemeModel::Node *createNode(ShaderBuilder *model, const QString &path) {
        AbstractSchemeModel::Node *result   = ShaderFunction::createNode(model, path);
        {
            AbstractSchemeModel::Item *out  = new AbstractSchemeModel::Item;
            out->name   = A;
            out->out    = false;
            out->pos    = 0;
            out->type   = QMetaType::QVector2D;
            result->list.push_back(out);
        }
        {
            AbstractSchemeModel::Item *out  = new AbstractSchemeModel::Item;
            out->name   = B;
            out->out    = false;
            out->pos    = 1;
            out->type   = QMetaType::QVector2D;
            result->list.push_back(out);
        }
        {
            AbstractSchemeModel::Item *out  = new AbstractSchemeModel::Item;
            out->name   = "";
            out->out    = true;
            out->pos    = 0;
            out->type   = QMetaType::QVector2D;
            result->list.push_back(out);
        }
        return result;
    }

    uint32_t compile(AbstractSchemeModel::Node *object, const QString &operation, QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        if(m_Position == -1) {
            QString args;

            vector<const char *> names    = {A, B};

            const AbstractSchemeModel::Link *l  = nullptr;

            for(uint8_t i = 0; i < 2; i++) {
                l   = m_pModel->findLink(object, names[i]);
                if(l) {
                    ShaderFunction *node   = static_cast<ShaderFunction *>(l->sender->ptr);
                    if(node) {
                        uint8_t type;
                        uint32_t index  = node->build(value, *l, depth, type);
                        if(i == 0) {
                            size    = type;
                        }
                        args    += ((i == 0) ? "" : operation) + convert("local" + QString::number(index), type, size);
                    }
                }
            }

            switch (size) {
                case QMetaType::QVector2D:  value.append("\tvec2"); break;
                case QMetaType::QVector3D:  value.append("\tvec3"); break;
                case QMetaType::QVector4D:  value.append("\tvec4"); break;
                default: value.append("\tfloat"); break;
            }
            value.append(QString(" local%1 = %2;\n").arg(depth).arg(args));
        }

        return ShaderFunction::build(value, link, depth, size);
    }
};

class Subtraction : public MathOperation {
    Q_OBJECT

public:
    Q_INVOKABLE Subtraction() {}

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        return compile(m_pNode, " - ", value, link, depth, size);
    }
};

class Add : public MathOperation {
    Q_OBJECT

public:
    Q_INVOKABLE Add() {}

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        return compile(m_pNode, " + ", value, link, depth, size);
    }
};

class Divide : public MathOperation {
    Q_OBJECT

public:
    Q_INVOKABLE Divide() {}

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        return compile(m_pNode, " / ", value, link, depth, size);
    }
};

class Multiply : public MathOperation {
    Q_OBJECT

public:
    Q_INVOKABLE Multiply() {}

    uint32_t build(QString &value, const AbstractSchemeModel::Link &link, uint32_t &depth, uint8_t &size) {
        return compile(m_pNode, " * ", value, link, depth, size);
    }
};

#endif // AMATHOPERATOR

