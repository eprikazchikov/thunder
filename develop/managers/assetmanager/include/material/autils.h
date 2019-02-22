#ifndef AUTILS
#define AUTILS

#include <components/material.h>

#define X   "X"
#define Y   "Y"
#define Z   "Z"
#define W   "W"

class VectorRutine : public IMaterialObjectGL {
public:
    static constexpr const char *IN = "In";

    void breakVector(AObject &object, string &value, const AObject::link_data &link, uint32_t &depth, uint8_t &size) {
        size    = AVariant::FLOAT;

        const AObject::link_data *l  = findLink(object, IN);
        if(l) {
            IMaterialObjectGL *node = dynamic_cast<IMaterialObjectGL *>(l->sender);
            if(node) {
                uint8_t type;
                node->build(value, *l, depth, type);
                depth++;

                uint8_t component = 0;
                if(link.signal == Y) {
                    component   = 1;
                } else if(link.signal == Z) {
                    component   = 2;
                } else if(link.signal == W) {
                    component   = 3;
                }

                value += "\tfloat local" + to_string(depth) + " = ";
                value += convert("local" + to_string(depth - 1), type, AVariant::FLOAT, component) + ";\n";
            }
        }
    }
};

class BreakVector2 : public VectorRutine {
    ACLASS(BreakVector2)
    AREGISTER(BreakVector2, Material/Utils)

public:
    BreakVector2() {
        APROPERTY(float,    IN,   "", 0.0f,   AProperty::WRITE | AProperty::SCHEME,  0);
        APROPERTY(float,    X,    "", 0.0f,   AProperty::READ  | AProperty::SCHEME,  0);
        APROPERTY(float,    Y,    "", 0.0f,   AProperty::READ  | AProperty::SCHEME,  1);
    }

    void                        build                   (string &value, const AObject::link_data &link, uint32_t &depth, uint8_t &size) {
        breakVector(*this, value, link, depth, size);
    }
};

class BreakVector3 : public VectorRutine {
    ACLASS(BreakVector3)
    AREGISTER(BreakVector3, Material/Utils)

public:
    BreakVector3() {
        APROPERTY(float,    IN,   "", 0.0f,   AProperty::WRITE | AProperty::SCHEME,  0);
        APROPERTY(float,    X,    "", 0.0f,   AProperty::READ  | AProperty::SCHEME,  0);
        APROPERTY(float,    Y,    "", 0.0f,   AProperty::READ  | AProperty::SCHEME,  1);
        APROPERTY(float,    Z,    "", 0.0f,   AProperty::READ  | AProperty::SCHEME,  2);
    }

    void                        build                   (string &value, const AObject::link_data &link, uint32_t &depth, uint8_t &size) {
        breakVector(*this, value, link, depth, size);
    }
};

class BreakVector4 : public VectorRutine {
    ACLASS(BreakVector4)
    AREGISTER(BreakVector4, Material/Utils)

public:
    BreakVector4() {
        APROPERTY(float,    IN,   "", 0.0f,   AProperty::WRITE | AProperty::SCHEME,  0);
        APROPERTY(float,    X,    "", 0.0f,   AProperty::READ  | AProperty::SCHEME,  0);
        APROPERTY(float,    Y,    "", 0.0f,   AProperty::READ  | AProperty::SCHEME,  1);
        APROPERTY(float,    Z,    "", 0.0f,   AProperty::READ  | AProperty::SCHEME,  2);
        APROPERTY(float,    W,    "", 0.0f,   AProperty::READ  | AProperty::SCHEME,  3);
    }

    void                        build                   (string &value, const AObject::link_data &link, uint32_t &depth, uint8_t &size) {
        breakVector(*this, value, link, depth, size);
    }
};

#endif // AUTILS

