#include "ctshaderprogram.h"
#include "ctopenglfunctions.h"
#include "ctmap.h"

#ifndef CT_OPENGL_ES2
static const char ct_disablePrecision[] =
    "#define lowp\n"
    "#define mediump\n"
    "#define highp\n";
#endif


class CtShaderProgramPrivate
{
public:
    CtShaderProgramPrivate();

    GLuint loadShader(GLenum type, const char *shaderSrc);

    GLuint id;
    bool linked;
    GLuint vertexShader;
    GLuint fragmentShader;
    CtMap<CtString, GLint> uniformLocations;
    CtMap<CtString, GLint> attributeLocations;
};

CtShaderProgramPrivate::CtShaderProgramPrivate()
    : id(0),
      linked(false),
      vertexShader(0),
      fragmentShader(0)
{

}

GLuint CtShaderProgramPrivate::loadShader(GLenum type, const char *shaderSrc)
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


CtShaderProgram::CtShaderProgram()
    : d(new CtShaderProgramPrivate)
{

}

CtShaderProgram::~CtShaderProgram()
{
    releaseVertexShader();
    releaseFragmentShader();
    unlink();
}

GLuint CtShaderProgram::id() const
{
    return d->id;
}

bool CtShaderProgram::isValid() const
{
    return d->linked;
}

bool CtShaderProgram::loadVertexShader(const CtString &source)
{
    CtString shaderSource;
#ifndef CT_OPENGL_ES2
    shaderSource += ct_disablePrecision;
#endif

    shaderSource += source;

    if (d->vertexShader)
        CtGL::glDeleteShader(d->vertexShader);

    d->vertexShader = d->loadShader(GL_VERTEX_SHADER, shaderSource.c_str());
    return (d->vertexShader != 0);
}

bool CtShaderProgram::loadFragmentShader(const CtString &source)
{
    CtString shaderSource;
#ifndef CT_OPENGL_ES2
    shaderSource += ct_disablePrecision;
#endif

    shaderSource += source;

    if (d->fragmentShader)
        CtGL::glDeleteShader(d->fragmentShader);

    d->fragmentShader = d->loadShader(GL_FRAGMENT_SHADER, shaderSource.c_str());
    return (d->fragmentShader != 0);
}

bool CtShaderProgram::link()
{
    if (d->linked)
        return true;

    d->linked = false;

    if (!d->vertexShader || !d->fragmentShader)
        return false;

    d->id = CtGL::glCreateProgram();

    if (!d->id)
        return false;

    CtGL::glAttachShader(d->id, d->vertexShader);
    CtGL::glAttachShader(d->id, d->fragmentShader);
    CtGL::glLinkProgram(d->id);

    GLint linked;
    CtGL::glGetProgramiv(d->id, GL_LINK_STATUS, &linked);

    if (!linked) {
        GLint infoLen = 0;
        CtGL::glGetProgramiv(d->id, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            char *infoLog = new char[infoLen];
            CtGL::glGetProgramInfoLog(d->id, infoLen, NULL, infoLog);
            CT_WARNING("CtShaderProgram::link: Error linking program:"
                       << CT_ENDL << infoLog);
            delete infoLog;
        }

        unlink();
        return false;
    }

    d->linked = true;
    return true;
}

bool CtShaderProgram::link(const CtString &vertexShader, const CtString &fragmentShader)
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

void CtShaderProgram::unlink()
{
    if (d->linked) {
        CtGL::glDeleteProgram(d->id);
        d->id = 0;
        d->linked = false;
        d->uniformLocations.clear();
        d->attributeLocations.clear();
    }
}

bool CtShaderProgram::bind()
{
    if (!d->linked)
        return false;

    CtGL::glUseProgram(d->id);
    return !CtGL::glGetError();
}

bool CtShaderProgram::release()
{
    if (!d->linked)
        return false;

    CtGL::glUseProgram(0);
    return !CtGL::glGetError();
}

void CtShaderProgram::releaseVertexShader()
{
    if (d->vertexShader) {
        CtGL::glDeleteShader(d->vertexShader);
        d->vertexShader = 0;
    }
}

void CtShaderProgram::releaseFragmentShader()
{
    if (d->fragmentShader) {
        CtGL::glDeleteShader(d->fragmentShader);
        d->fragmentShader = 0;
    }
}

int CtShaderProgram::uniformLocation(const char *name) const
{
    if (!d->linked)
        return -1;

    CtMap<CtString, GLint>::iterator it = d->uniformLocations.find(name);

    if (it != d->uniformLocations.end())
        return it->second;

    GLint location = CtGL::glGetUniformLocation(d->id, name);
    d->uniformLocations.insert(name, location);

    return location;
}

int CtShaderProgram::attributeLocation(const char *name) const
{
    if (!d->linked)
        return -1;

    CtMap<CtString, GLint>::iterator it = d->attributeLocations.find(name);

    if (it != d->attributeLocations.end())
        return it->second;

    GLint location = CtGL::glGetAttribLocation(d->id, name);
    d->attributeLocations.insert(name, location);

    return location;
}

void CtShaderProgram::setVertexAttributePointer(int location, GLenum type,
                                                int size, int stride, const void *values)
{
    if (location >= 0)
        CtGL::glVertexAttribPointer(location, size, type, GL_FALSE, stride, values);
}

void CtShaderProgram::setVertexAttributePointer(int location, int size, const GLfloat *values)
{
    if (location >= 0)
        CtGL::glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, 0,
                                    reinterpret_cast<const void *>(values));
}

void CtShaderProgram::enableVertexAttributeArray(int location)
{
    if (location >= 0)
        CtGL::glEnableVertexAttribArray(location);
}

void CtShaderProgram::setUniformValue(int location, GLint v0)
{
    if (location >= 0)
        CtGL::glUniform1i(location, v0);
}

void CtShaderProgram::setUniformValue(int location, GLint v0, GLint v1)
{
    if (location >= 0)
        CtGL::glUniform2i(location, v0, v1);
}

void CtShaderProgram::setUniformValue(int location, GLint v0, GLint v1, GLint v2)
{
    if (location >= 0)
        CtGL::glUniform3i(location, v0, v1, v2);
}

void CtShaderProgram::setUniformValue(int location, GLint v0, GLint v1, GLint v2, GLint v3)
{
    if (location >= 0)
        CtGL::glUniform4i(location, v0, v1, v2, v3);
}

void CtShaderProgram::setUniformValue(int location, GLfloat value)
{
    if (location >= 0)
        CtGL::glUniform1f(location, value);
}

void CtShaderProgram::setUniformValue(int location, GLfloat v0, GLfloat v1)
{
    if (location >= 0)
        CtGL::glUniform2f(location, v0, v1);
}

void CtShaderProgram::setUniformValue(int location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    if (location >= 0)
        CtGL::glUniform3f(location, v0, v1, v2);
}

void CtShaderProgram::setUniformValue(int location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    if (location >= 0)
        CtGL::glUniform4f(location, v0, v1, v2, v3);
}

void CtShaderProgram::setUniformValue(int location, const CtMatrix &matrix)
{
    if (location >= 0)
        CtGL::glUniformMatrix4fv(location, 1, GL_FALSE,
                                 reinterpret_cast<GLfloat *>(matrix.data()));
}
