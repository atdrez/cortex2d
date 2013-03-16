#include "ctshaderprogram.h"
#include "ctopenglfunctions.h"
#include "ctmap.h"

#ifndef CT_OPENGL_ES2
static const char ct_disablePrecision[] =
    "#define lowp\n"
    "#define mediump\n"
    "#define highp\n";
#endif

GLuint CtGpuProgram::loadShader(GLenum type, const char *shaderSrc)
{
    GLuint shader = CtGL::glCreateShader(type);

    if (!shader)
        return 0;

    // Load the shader source
    CtGL::glShaderSource(shader, 1, &shaderSrc, NULL);

    // Compile the shader
    CtGL::glCompileShader(shader);

    // Check the compile status
    GLint compiled;
    CtGL::glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLint infoLen = 0;
        CtGL::glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            char *infoLog = new char[infoLen];
            CtGL::glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            CT_WARNING("Error compiling shader ('" << infoLog << "'). Source: "
                       << CT_ENDL << shaderSrc);
            delete infoLog;
        }

        CtGL::glDeleteShader(shader);
        return 0;
    }

    return shader;
}


CtGpuProgram::CtGpuProgram()
    : mId(0),
      mIsLinked(false),
      mVertexShader(0),
      mFragmentShader(0)
{

}

CtGpuProgram::~CtGpuProgram()
{
    releaseVertexShader();
    releaseFragmentShader();
    unlink();
}

bool CtGpuProgram::loadVertexShader(const CtString &source)
{
    CtString shaderSource;
#ifndef CT_OPENGL_ES2
    shaderSource += ct_disablePrecision;
#endif

    shaderSource += source;

    if (mVertexShader)
        CtGL::glDeleteShader(mVertexShader);

    mVertexShader = loadShader(GL_VERTEX_SHADER, shaderSource.data());
    return (mVertexShader != 0);
}

bool CtGpuProgram::loadFragmentShader(const CtString &source)
{
    CtString shaderSource;
#ifndef CT_OPENGL_ES2
    shaderSource += ct_disablePrecision;
#endif

    shaderSource += source;

    if (mFragmentShader)
        CtGL::glDeleteShader(mFragmentShader);

    mFragmentShader = loadShader(GL_FRAGMENT_SHADER, shaderSource.data());
    return (mFragmentShader != 0);
}

bool CtGpuProgram::link()
{
    if (mIsLinked)
        return true;

    mIsLinked = false;

    if (!mVertexShader || !mFragmentShader)
        return false;

    mId = CtGL::glCreateProgram();

    if (!mId)
        return false;

    CtGL::glAttachShader(mId, mVertexShader);
    CtGL::glAttachShader(mId, mFragmentShader);
    CtGL::glLinkProgram(mId);

    GLint linked;
    CtGL::glGetProgramiv(mId, GL_LINK_STATUS, &linked);

    if (!linked) {
        GLint infoLen = 0;
        CtGL::glGetProgramiv(mId, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            char *infoLog = new char[infoLen];
            CtGL::glGetProgramInfoLog(mId, infoLen, NULL, infoLog);
            CT_WARNING("CtGpuProgram::link: Error linking program:"
                       << CT_ENDL << infoLog);
            delete infoLog;
        }

        unlink();
        return false;
    }

    mIsLinked = true;
    return true;
}

bool CtGpuProgram::link(const CtString &vertexShader, const CtString &fragmentShader)
{
    unlink();

    if (!loadVertexShader(vertexShader))
        return false;

    if (!loadFragmentShader(fragmentShader)) {
        releaseVertexShader();
        return false;
    }

    bool result = link();

    releaseVertexShader();
    releaseFragmentShader();

    return result;
}

void CtGpuProgram::unlink()
{
    if (mIsLinked) {
        CtGL::glDeleteProgram(mId);
        mId = 0;
        mIsLinked = false;
        mUniformLocations.clear();
        mAttributeLocations.clear();
    }
}

bool CtGpuProgram::bind()
{
    if (!mIsLinked)
        return false;

    CtGL::glUseProgram(mId);
    return !CtGL::glGetError();
}

bool CtGpuProgram::release()
{
    if (!mIsLinked)
        return false;

    CtGL::glUseProgram(0);
    return !CtGL::glGetError();
}

void CtGpuProgram::releaseVertexShader()
{
    if (mVertexShader) {
        CtGL::glDeleteShader(mVertexShader);
        mVertexShader = 0;
    }
}

void CtGpuProgram::releaseFragmentShader()
{
    if (mFragmentShader) {
        CtGL::glDeleteShader(mFragmentShader);
        mFragmentShader = 0;
    }
}

int CtGpuProgram::uniformLocation(const char *name) const
{
    if (!mIsLinked)
        return -1;

    CtMap<CtString, GLint>::const_iterator it = mUniformLocations.find(name);

    if (it != mUniformLocations.end())
        return it->second;

    GLint location = CtGL::glGetUniformLocation(mId, name);
    mUniformLocations.insert(name, location);

    return location;
}

int CtGpuProgram::attributeLocation(const char *name) const
{
    if (!mIsLinked)
        return -1;

    CtMap<CtString, GLint>::const_iterator it = mAttributeLocations.find(name);

    if (it != mAttributeLocations.end())
        return it->second;

    GLint location = CtGL::glGetAttribLocation(mId, name);
    mAttributeLocations.insert(name, location);

    return location;
}

void CtGpuProgram::setVertexAttributePointer(int location, GLenum type,
                                                int size, int stride, const void *values)
{
    if (location >= 0)
        CtGL::glVertexAttribPointer(location, size, type, GL_FALSE, stride, values);
}

void CtGpuProgram::setVertexAttributePointer(int location, int size, const GLfloat *values)
{
    if (location >= 0)
        CtGL::glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, 0,
                                    reinterpret_cast<const void *>(values));
}

void CtGpuProgram::enableVertexAttributeArray(int location)
{
    if (location >= 0)
        CtGL::glEnableVertexAttribArray(location);
}

void CtGpuProgram::setUniformValue(int location, GLint v0)
{
    if (location >= 0)
        CtGL::glUniform1i(location, v0);
}

void CtGpuProgram::setUniformValue(int location, GLint v0, GLint v1)
{
    if (location >= 0)
        CtGL::glUniform2i(location, v0, v1);
}

void CtGpuProgram::setUniformValue(int location, GLint v0, GLint v1, GLint v2)
{
    if (location >= 0)
        CtGL::glUniform3i(location, v0, v1, v2);
}

void CtGpuProgram::setUniformValue(int location, GLint v0, GLint v1, GLint v2, GLint v3)
{
    if (location >= 0)
        CtGL::glUniform4i(location, v0, v1, v2, v3);
}

void CtGpuProgram::setUniformValue(int location, GLfloat value)
{
    if (location >= 0)
        CtGL::glUniform1f(location, value);
}

void CtGpuProgram::setUniformValue(int location, GLfloat v0, GLfloat v1)
{
    if (location >= 0)
        CtGL::glUniform2f(location, v0, v1);
}

void CtGpuProgram::setUniformValue(int location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    if (location >= 0)
        CtGL::glUniform3f(location, v0, v1, v2);
}

void CtGpuProgram::setUniformValue(int location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    if (location >= 0)
        CtGL::glUniform4f(location, v0, v1, v2, v3);
}

void CtGpuProgram::setUniformValue(int location, const CtMatrix4x4 &matrix)
{
    if (location >= 0)
        CtGL::glUniformMatrix4fv(location, 1, GL_FALSE, (GLfloat *)matrix.constData());
}
