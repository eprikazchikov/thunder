#ifndef AGRADIENT_H
#define AGRADIENT_H

#include "../shaderbuilder.h"

#define X       "X"
#define Y       "Y"
#define A       "A"
#define EDGE1   "Edge1"
#define EDGE2   "Edge2"

class ASmothCurve : public AObject, public IShaderFunction {
    ACLASS(ASmothCurve)
    AREGISTER(ASmothCurve, Material/Gradient)

    static constexpr const char *OUT ="Out";
public:
    ASmothCurve() {
        APROPERTY(float,    OUT,      "", 1.0f, AProperty::READ | AProperty::SCHEME,   -1);
        APROPERTY(float,    EDGE1,    "", 0.0f, AProperty::WRITE | AProperty::SCHEME,   0);
        APROPERTY(float,    EDGE2,    "", 1.0f, AProperty::WRITE | AProperty::SCHEME,   1);
        APROPERTY(float,    X,        "", 1.0f, AProperty::WRITE | AProperty::SCHEME,   2);
    }

    void                        build                   (string &value, const AObject::link_data &, uint32_t &depth, uint8_t &size) {
        string arg1;
        string arg2;
        string arg3;

        const link_data *l;
/*
        l   = findLink(*this, EDGE1);
        if(l) {
            IMaterialObjectGL *node = dynamic_cast<IMaterialObjectGL *>(l->sender);
            if(node) {
                node->build(value, *l, depth, size);
                arg1 = "local" + to_string(depth);
                depth++;
            }
        }
        l   = findLink(*this, EDGE2);
        if(l) {
            IMaterialObjectGL *node = dynamic_cast<IMaterialObjectGL *>(l->sender);
            if(node) {
                node->build(value, *l, depth, 1);
                arg2 = "local" + to_string(depth);
                depth++;
            }
        }
        l   = findLink(*this, X);
        if(l) {
            IMaterialObjectGL *node = dynamic_cast<IMaterialObjectGL *>(l->sender);
            if(node) {
                node->build(value, *l, depth, 1);
                arg3 = "local" + to_string(depth);
                depth++;
            }
        }

        stringstream lt;
        lt << "\tfloat lf" << depth << " = smoothstep(" << arg1 << ", " << arg2 << ", " << arg3 << ");\n";

        value.append(lt.str());

        stringstream local;
        switch (size) {
            case 1: {
                local << "\tfloat local" << depth << " = lf" << depth << ";\n";
            } break;
            case 2: {
                local << "\tvec2 local" << depth << " = vec2(lf" << depth << ");\n";
            } break;
            case 3: {
                local << "\tvec3 local" << depth << " = vec3(lf" << depth << ");\n";
            } break;
            case 4: {
                local << "\tvec4 local" << depth << " = vec4(lf" << depth << ");\n";
            } break;
            default: break;
        }

        value.append(local.str());
*/
    }
};

class ALinearGradient : public AObject, public IShaderObject {
    ACLASS(ALinearGradient)
    AREGISTER(ALinearGradient, Material/Gradient)

public:
    ALinearGradient() {
        APROPERTY(float,    OUT,      "", 1.0f, AProperty::READ | AProperty::SCHEME,   -1);
        APROPERTY(float,    X,        "", 0.0f, AProperty::WRITE | AProperty::SCHEME,   0);
        APROPERTY(float,    Y,        "", 1.0f, AProperty::WRITE | AProperty::SCHEME,   1);
        APROPERTY(float,    A,        "", 1.0f, AProperty::WRITE | AProperty::SCHEME,   2);
    }

    void                        build                   (string &value, const AObject::link_data &, uint32_t &depth, uint8_t &size) {
        string arg1;
        string arg2;
        string arg3;

        const link_data *l;
/*
        l   = findLink(*this, X);
        if(l) {
            IMaterialObjectGL *node = dynamic_cast<IMaterialObjectGL *>(l->sender);
            if(node) {
                node->build(value, *l, depth, 1);
                arg1 = "local" + to_string(depth);
                depth++;
            }
        }
        l   = findLink(*this, Y);
        if(l) {
            IMaterialObjectGL *node = dynamic_cast<IMaterialObjectGL *>(l->sender);
            if(node) {
                node->build(value, *l, depth, 1);
                arg2 = "local" + to_string(depth);
                depth++;
            }
        }
        l   = findLink(*this, A);
        if(l) {
            IMaterialObjectGL *node = dynamic_cast<IMaterialObjectGL *>(l->sender);
            if(node) {
                node->build(value, *l, depth, 1);
                arg3 = "local" + to_string(depth);
                depth++;
            }
        }

        stringstream lt;
        lt << "\tfloat lf" << depth << " = mix(" << arg1 << ", " << arg2 << ", " << arg3 << ");\n";

        value.append(lt.str());

        stringstream local;
        switch (size) {
            case 1: {
                local << "\tfloat local" << depth << " = lf" << depth << ";\n";
            } break;
            case 2: {
                local << "\tvec2 local" << depth << " = vec2(lf" << depth << ");\n";
            } break;
            case 3: {
                local << "\tvec3 local" << depth << " = vec3(lf" << depth << ");\n";
            } break;
            case 4: {
                local << "\tvec4 local" << depth << " = vec4(lf" << depth << ");\n";
            } break;
            default: break;
        }
        value.append(local.str());
*/
    }
};

#endif // AGRADIENT_H
