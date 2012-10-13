#ifndef FLSHADERPROGRAM_H
#define FLSHADERPROGRAM_H

#include "flGL.h"
#include "flglobal.h"

class FlShaderProgramPrivate;

class FlShaderProgram
{
public:
    FlShaderProgram();
    virtual ~FlShaderProgram();

    GLuint id() const;
    bool isValid() const;

    bool loadVertexShader(const FlString &source);
    void releaseVertexShader();

    bool loadFragmentShader(const FlString &source);
    void releaseFragmentShader();

    bool link();
    void unlink();

    bool bind();
    bool release();

private:
    FlShaderProgramPrivate *d;
};

#endif
