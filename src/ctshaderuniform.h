#ifndef CTSHADERUNIFORM_H
#define CTSHADERUNIFORM_H

#include "ctglobal.h"

class CtShaderEffect;

class CtShaderUniform
{
public:
    enum Type {
        IntType,
        FloatType,
        Vec2Type
    };

    inline CtShaderUniform(const CtString &name, Type type);

    Type type() const { return m_type; }
    CtString name() const { return m_name; }

    void setValue(int value) { m.ivalue = value; }
    void setValue(ctreal value) { m.rvalue = value; }
    void setValue(ctreal v1, ctreal v2) { m.vec2.r1 = v1; m.vec2.r2 = v2; }

private:
    union {
        int ivalue;
        ctreal rvalue;
        struct {
            ctreal r1;
            ctreal r2;
        } vec2;
    } m;

    Type m_type;
    CtString m_name;
    GLint m_location;

    friend class CtShaderEffect;
};

CtShaderUniform::CtShaderUniform(const CtString &name, Type type)
    : m_type(type),
      m_name(name),
      m_location(-1)
{
}

#endif
