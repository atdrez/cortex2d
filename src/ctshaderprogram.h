#ifndef CTSHADERPROGRAM_H
#define CTSHADERPROGRAM_H

#include "ctGL.h"
#include "ctglobal.h"

class CtShaderProgramPrivate;

class CtShaderProgram
{
public:
    CtShaderProgram();
    virtual ~CtShaderProgram();

    GLuint id() const;
    bool isValid() const;

    bool loadVertexShader(const CtString &source);
    void releaseVertexShader();

    bool loadFragmentShader(const CtString &source);
    void releaseFragmentShader();

    bool link();
    void unlink();

    bool bind();
    bool release();

private:
    CtShaderProgramPrivate *d;
};

#endif
