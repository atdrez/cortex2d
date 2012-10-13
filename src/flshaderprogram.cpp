#include "flshaderprogram.h"
#include "flopenglfunctions.h"

#ifndef FL_OPENGL_ES2
static const char fl_disablePrecision[] =
    "#define lowp\n"
    "#define mediump\n"
    "#define highp\n";
#endif

class FlShaderProgramPrivate
{
public:
    FlShaderProgramPrivate();

    GLuint loadShader(GLenum type, const char *shaderSrc);

    GLuint id;
    bool linked;
    GLuint vertexShader;
    GLuint fragmentShader;
};

FlShaderProgramPrivate::FlShaderProgramPrivate()
    : id(0),
      linked(false),
      vertexShader(0),
      fragmentShader(0)
{

}

GLuint FlShaderProgramPrivate::loadShader(GLenum type, const char *shaderSrc)
{
    GLuint shader = FlGL::glCreateShader(type);

    if (!shader)
        return 0;

    // Load the shader source
    FlGL::glShaderSource(shader, 1, &shaderSrc, NULL);

    // Compile the shader
    FlGL::glCompileShader(shader);

    // Check the compile status
    GLint compiled;
    FlGL::glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLint infoLen = 0;
        FlGL::glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            char *infoLog = new char[infoLen];
            FlGL::glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            FL_WARNING("Error compiling shader ('" << infoLog << "'). Source: "
                       << FL_ENDL << shaderSrc);
            delete infoLog;
        }

        FlGL::glDeleteShader(shader);
        return 0;
    }

    return shader;
}


FlShaderProgram::FlShaderProgram()
    : d(new FlShaderProgramPrivate)
{

}

FlShaderProgram::~FlShaderProgram()
{
    releaseVertexShader();
    releaseFragmentShader();
    unlink();
}

GLuint FlShaderProgram::id() const
{
    return d->id;
}

bool FlShaderProgram::isValid() const
{
    return d->linked;
}

bool FlShaderProgram::loadVertexShader(const FlString &source)
{
    FlString shaderSource;
#ifndef FL_OPENGL_ES2
    shaderSource += fl_disablePrecision;
#endif

    shaderSource += source;

    if (d->vertexShader)
        FlGL::glDeleteShader(d->vertexShader);

    d->vertexShader = d->loadShader(GL_VERTEX_SHADER, shaderSource.c_str());
    return (d->vertexShader != 0);
}

bool FlShaderProgram::loadFragmentShader(const FlString &source)
{
    FlString shaderSource;
#ifndef FL_OPENGL_ES2
    shaderSource += fl_disablePrecision;
#endif

    shaderSource += source;

    if (d->fragmentShader)
        FlGL::glDeleteShader(d->fragmentShader);

    d->fragmentShader = d->loadShader(GL_FRAGMENT_SHADER, shaderSource.c_str());
    return (d->fragmentShader != 0);
}

bool FlShaderProgram::link()
{
    d->linked = false;

    if (!d->vertexShader || !d->fragmentShader)
        return false;

    d->id = FlGL::glCreateProgram();

    if (!d->id)
        return false;

    FlGL::glAttachShader(d->id, d->vertexShader);
    FlGL::glAttachShader(d->id, d->fragmentShader);
    FlGL::glLinkProgram(d->id);

    GLint linked;
    FlGL::glGetProgramiv(d->id, GL_LINK_STATUS, &linked);

    if (!linked) {
        GLint infoLen = 0;
        FlGL::glGetProgramiv(d->id, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            char *infoLog = new char[infoLen];
            FlGL::glGetProgramInfoLog(d->id, infoLen, NULL, infoLog);
            FL_WARNING("FlShaderProgram::link: Error linking program:"
                       << FL_ENDL << infoLog);
            delete infoLog;
        }

        unlink();
        return false;
    }

    d->linked = true;
    return true;
}

void FlShaderProgram::unlink()
{
    if (d->linked) {
        FlGL::glDeleteProgram(d->id);
        d->id = 0;
        d->linked = false;
    }
}

bool FlShaderProgram::bind()
{
    if (!d->linked)
        return false;

    FlGL::glUseProgram(d->id);
    return !FlGL::glGetError();
}

bool FlShaderProgram::release()
{
    if (!d->linked)
        return false;

    FlGL::glUseProgram(0);
    return !FlGL::glGetError();
}

void FlShaderProgram::releaseVertexShader()
{
    if (d->vertexShader) {
        FlGL::glDeleteShader(d->vertexShader);
        d->vertexShader = 0;
    }
}

void FlShaderProgram::releaseFragmentShader()
{
    if (d->fragmentShader) {
        FlGL::glDeleteShader(d->fragmentShader);
        d->fragmentShader = 0;
    }
}
