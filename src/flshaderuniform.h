#ifndef FLSHADERUNIFORM_H
#define FLSHADERUNIFORM_H

#include "flglobal.h"

class FlShaderEffect;

class FlShaderUniform
{
public:
    enum Type {
        IntType,
        FloatType,
        Vec2Type
    };

    inline FlShaderUniform(const FlString &name, Type type);

    Type type() const { return m_type; }
    FlString name() const { return m_name; }

    void setValue(int value) { m.ivalue = value; }
    void setValue(flreal value) { m.rvalue = value; }
    void setValue(flreal v1, flreal v2) { m.vec2.r1 = v1; m.vec2.r2 = v2; }

private:
    union {
        int ivalue;
        flreal rvalue;
        struct {
            flreal r1;
            flreal r2;
        } vec2;
    } m;

    Type m_type;
    FlString m_name;
    GLint m_location;

    friend class FlShaderEffect;
};

FlShaderUniform::FlShaderUniform(const FlString &name, Type type)
    : m_type(type),
      m_name(name),
      m_location(-1)
{
}

#endif
