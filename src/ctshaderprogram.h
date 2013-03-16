#ifndef CTSHADERPROGRAM_H
#define CTSHADERPROGRAM_H

#include <stdio.h>
#include "ctGL.h"
#include "ctmap.h"
#include "ctglobal.h"
#include "ctmatrix.h"

class CtGpuProgramPrivate;

class CtGpuProgram
{
public:
    CtGpuProgram();
    virtual ~CtGpuProgram();

    inline GLuint id() const { return mId; }
    inline bool isValid() const { return mIsLinked; }

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

    void setUniformValue(int location, const CtMatrix4x4 &matrix);

    static CtGpuProgram *sharedSolidShaderProgram();
    static CtGpuProgram *sharedTextureShaderProgram();
    static CtGpuProgram *sharedTextShaderProgram();
    static CtGpuProgram *sharedFragmentShaderProgram();
    static CtGpuProgram *sharedParticleShaderProgram();

private:
    GLuint loadShader(GLenum type, const char *shaderSrc);

    GLuint mId;
    bool mIsLinked;
    GLuint mVertexShader;
    GLuint mFragmentShader;
    mutable CtMap<CtString, GLint> mUniformLocations;
    mutable CtMap<CtString, GLint> mAttributeLocations;
};

#endif
