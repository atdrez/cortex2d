#ifndef CTSHADERPROGRAM_H
#define CTSHADERPROGRAM_H

#include "ctGL.h"
#include "ctglobal.h"
#include "ctmatrix.h"

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
    bool link(const CtString &vertexShader, const CtString &fragmentShader);
    void unlink();

    bool bind();
    bool release();

    int uniformLocation(const char *name) const;
    int attributeLocation(const char *name) const;

    void setVertexAttributePointer(int location, int size, const GLfloat *values);

    void setVertexAttributePointer(int location, GLenum type,
                                   int size, int stride, const void *values);

    void enableVertexAttributeArray(int location);

    void setUniformValue(int location, GLint v0);
    void setUniformValue(int location, GLint v0, GLint v1);
    void setUniformValue(int location, GLint v0, GLint v1, GLint v2);
    void setUniformValue(int location, GLint v0, GLint v1, GLint v2, GLint v3);

    void setUniformValue(int location, GLfloat v0);
    void setUniformValue(int location, GLfloat v0, GLfloat v1);
    void setUniformValue(int location, GLfloat v0, GLfloat v1, GLfloat v2);
    void setUniformValue(int location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

    void setUniformValue(int location, const CtMatrix &matrix);

    static CtShaderProgram *sharedSolidShaderProgram();
    static CtShaderProgram *sharedTextureShaderProgram();
    static CtShaderProgram *sharedTextShaderProgram();
    static CtShaderProgram *sharedParticleShaderProgram();

private:
    CtShaderProgramPrivate *d;
};

#endif
