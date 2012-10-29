#include "ctshaderprogram.h"
#include "ctopenglfunctions.h"

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

void CtShaderProgram::unlink()
{
    if (d->linked) {
        CtGL::glDeleteProgram(d->id);
        d->id = 0;
        d->linked = false;
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
