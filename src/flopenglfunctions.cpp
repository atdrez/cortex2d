#include "flopenglfunctions.h"

static char *FlGL_loadTGA(char *fileName, int *width, int *height)
{
    FILE *fp = fopen(fileName, "rb");

    if (!fp)
        return 0;

    unsigned char data[18];

    if (!fread(&data, sizeof(data), 1, fp)) {
        fclose(fp);
        return 0;
    }

    unsigned int w = *width = data[13] * 256 + data[12];
    unsigned int h = *height = data[15] * 256 + data[14];
    unsigned int bufferSize = data[16] / 8 * w * h;

    char *buffer = new char[bufferSize];

    if (buffer) {
        if (fread(buffer, 1, bufferSize, fp) != bufferSize) {
            delete [] buffer;
            buffer = 0;
        }
    }

    fclose(fp);
    return buffer;
}

GLuint FlGL::loadTGATexture(const char *fileName)
{
    int w, h;
    GLuint texture;
    char *buffer = FlGL_loadTGA((char *)fileName, &w, &h);

    if (!buffer) {
        FL_WARNING("Error loading image '" << fileName << "'");
        return 0;
    }

    FlGL::glGenTextures(1, &texture);
    FlGL::glBindTexture(GL_TEXTURE_2D, texture);
    FlGL::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);

    FlGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    FlGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    FlGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    FlGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    delete buffer;

    return texture;
}

void FlGL::resetGlError()
{
    while (FlGL::glGetError() != GL_NO_ERROR);
}

FlString FlGL::getGlErrorMessage(GLenum error)
{
    switch (error) {
    case GL_NO_ERROR:
        return "No error";
    case GL_INVALID_ENUM:
        return "Invalid ENUM: An unacceptable value was specified for an enum argument";
    case GL_INVALID_VALUE:
        return "Invalid value: A numeric argument is out of range";
    case GL_INVALID_OPERATION:
        return "Invalid operation: The specified operation is not allowed in the current state";
/*
    case  GL_STACK_OVERFLOW:
        return "Stack overflow: A command caused a stack overflow";
    case GL_STACK_UNDERFLOW:
        return "Stack underflow: A command caused a stack underflow";
    case GL_OUT_OF_MEMORY:
        return "Out of memory: There is not enough memory left to execute the command";
    case  GL_TABLE_TOO_LARGE:
        return "Table too large: The specified table exceeds the maximum supported size";
*/
    default:
        return "Unknown GL error";
    }
}

/******************************************************************************
 * OpenGL ES wrappers
 *****************************************************************************/

#define FL_GL_FPTR(ret, name, params)                            \
    typedef ret (FL_GL_APIENTRYP Func_##name)params;             \
    static Func_##name fl_##name = name;

FL_GL_FPTR(GLuint, glCreateShader, (GLenum  shaderType))
FL_GL_FPTR(void, glDeleteShader, (GLenum  shaderType))
FL_GL_FPTR(void, glShaderSource, (GLuint shader, GLsizei count, const GLchar **string, const GLint *length))
FL_GL_FPTR(void, glCompileShader, (GLenum  shaderType))
FL_GL_FPTR(void, glGetShaderiv, (GLuint shader,  GLenum pname,  GLint *params))
FL_GL_FPTR(void, glGetShaderInfoLog, (GLuint shader,  GLsizei maxLength,  GLsizei *length,  GLchar *infoLog))
FL_GL_FPTR(GLuint, glCreateProgram, (void))
FL_GL_FPTR(void, glDeleteProgram, (GLuint program))
FL_GL_FPTR(void, glAttachShader, (GLuint program,  GLuint shader))
FL_GL_FPTR(void, glLinkProgram, (GLuint program))
FL_GL_FPTR(void, glGetProgramiv, (GLuint program,  GLenum pname,  GLint *params))
FL_GL_FPTR(void, glGetProgramInfoLog, (GLuint program,  GLsizei maxLength,  GLsizei *length,  GLchar *infoLog))
FL_GL_FPTR(void, glViewport, (GLint x,  GLint y,  GLsizei width,  GLsizei height))
FL_GL_FPTR(void, glUseProgram, (GLuint program))
FL_GL_FPTR(void, glVertexAttribPointer, (GLuint index,  GLint size,  GLenum type,  GLboolean normalized,
                                         GLsizei stride,  const GLvoid * pointer))
FL_GL_FPTR(void, glEnableVertexAttribArray, (GLuint index))
FL_GL_FPTR(void, glDrawArrays, (GLenum mode,  GLint first,  GLsizei count))
FL_GL_FPTR(void, glClear, (GLbitfield mask))
FL_GL_FPTR(GLenum, glGetError, (void))
FL_GL_FPTR(void, glGenTextures, (GLsizei n, GLuint *textures))
FL_GL_FPTR(void, glBindTexture, (GLenum target,  GLuint texture))
FL_GL_FPTR(void, glTexImage2D, (GLenum target, GLint level, GLint internalFormat,
                                GLsizei width, GLsizei height,  GLint border,  GLenum format,
                                GLenum  type, const GLvoid *  data))
FL_GL_FPTR(void, glTexParameteri, (GLenum  target,  GLenum  pname,  GLint  param))
FL_GL_FPTR(void, glTexParameterf, (GLenum  target,  GLenum  pname,  GLfloat  param))

FL_GL_FPTR(void, glUniform1i, (GLint location, GLint v0))
FL_GL_FPTR(void, glUniform2i, (GLint location, GLint v0, GLint v1))
FL_GL_FPTR(void, glUniform3i, (GLint location, GLint v0, GLint v1, GLint v2))
FL_GL_FPTR(void, glUniform4i, (GLint location, GLint v0, GLint v1, GLint v2, GLint v3))

FL_GL_FPTR(void, glUniform1f, (GLint location, GLfloat v0))
FL_GL_FPTR(void, glUniform2f, (GLint location, GLfloat v0, GLfloat v1))
FL_GL_FPTR(void, glUniform3f, (GLint location, GLfloat v0, GLfloat v1, GLfloat v2))
FL_GL_FPTR(void, glUniform4f, (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3))

FL_GL_FPTR(GLint, glGetAttribLocation, (GLuint program,  const GLchar *name))
FL_GL_FPTR(GLint, glGetUniformLocation, (GLuint program,  const GLchar *name))
FL_GL_FPTR(void, glActiveTexture, (GLenum  texture))
FL_GL_FPTR(void, glUniformMatrix4fv, (GLint  location,  GLsizei  count,  GLboolean  transpose,  const GLfloat * value))
FL_GL_FPTR(void, glClearColor, (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha))
FL_GL_FPTR(void, glEnable, (GLenum cap))
FL_GL_FPTR(void, glBlendFunc, (GLenum  sfactor,  GLenum  dfactor))
FL_GL_FPTR(void, glDeleteTextures, (GLsizei n, const GLuint *textures))
FL_GL_FPTR(const GLubyte *, glGetString, (GLenum  name))
FL_GL_FPTR(void, glCompressedTexImage2D, (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize,  const GLvoid *data))
FL_GL_FPTR(void, glDrawElements, (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices))

#undef FL_GL_FPTR

void FlGL::assignFunctions(Func_GetProcAddress f)
{
// We don't need to change this pointers for Android
#ifndef FL_ANDROID

    if (!f)
        return;

#   define FL_GL_FASSIGN(name)                              \
    fl_##name = (Func_##name)f(#name);                      \
    FL_ASSERT(!fl_##name, "Unable to load " #name " function");

    FL_GL_FASSIGN(glCreateShader);
    FL_GL_FASSIGN(glDeleteShader);
    FL_GL_FASSIGN(glShaderSource);
    FL_GL_FASSIGN(glCompileShader);
    FL_GL_FASSIGN(glGetShaderiv);
    FL_GL_FASSIGN(glGetShaderInfoLog);
    FL_GL_FASSIGN(glCreateProgram);
    FL_GL_FASSIGN(glDeleteProgram);
    FL_GL_FASSIGN(glAttachShader);
    FL_GL_FASSIGN(glLinkProgram);
    FL_GL_FASSIGN(glGetProgramiv);
    FL_GL_FASSIGN(glGetProgramInfoLog);
    FL_GL_FASSIGN(glViewport);
    FL_GL_FASSIGN(glUseProgram);
    FL_GL_FASSIGN(glVertexAttribPointer);
    FL_GL_FASSIGN(glEnableVertexAttribArray);
    FL_GL_FASSIGN(glDrawArrays);
    FL_GL_FASSIGN(glClear);
    FL_GL_FASSIGN(glGetError);
    FL_GL_FASSIGN(glGenTextures);
    FL_GL_FASSIGN(glBindTexture);
    FL_GL_FASSIGN(glTexImage2D);
    FL_GL_FASSIGN(glTexParameteri);
    FL_GL_FASSIGN(glTexParameterf);
    FL_GL_FASSIGN(glUniform1i);
    FL_GL_FASSIGN(glUniform2i);
    FL_GL_FASSIGN(glUniform3i);
    FL_GL_FASSIGN(glUniform4i);
    FL_GL_FASSIGN(glUniform1f);
    FL_GL_FASSIGN(glUniform2f);
    FL_GL_FASSIGN(glUniform3f);
    FL_GL_FASSIGN(glUniform4f);
    FL_GL_FASSIGN(glGetAttribLocation);
    FL_GL_FASSIGN(glGetUniformLocation);
    FL_GL_FASSIGN(glActiveTexture);
    FL_GL_FASSIGN(glUniformMatrix4fv);
    FL_GL_FASSIGN(glClearColor);
    FL_GL_FASSIGN(glEnable);
    FL_GL_FASSIGN(glBlendFunc);
    FL_GL_FASSIGN(glDeleteTextures);
    FL_GL_FASSIGN(glGetString);
    FL_GL_FASSIGN(glCompressedTexImage2D);
    FL_GL_FASSIGN(glDrawElements);

#   undef FL_GL_FASSIGN

#endif // FL_ANDROID
}


GLuint FlGL::glCreateShader(GLenum shaderType)
{
    return fl_glCreateShader(shaderType);
}

void FlGL::glDeleteShader(GLenum shaderType)
{
    return fl_glDeleteShader(shaderType);
}

void FlGL::glShaderSource(GLuint shader,  GLsizei count, const GLchar **string, const GLint *length)
{
    return fl_glShaderSource(shader, count, string, length);
}

void FlGL::glCompileShader(GLenum shaderType)
{
    return fl_glCompileShader(shaderType);
}

void FlGL::glGetShaderiv(GLuint shader,  GLenum pname,  GLint *params)
{
    return fl_glGetShaderiv(shader, pname, params);
}

void FlGL::glGetShaderInfoLog(GLuint shader,  GLsizei maxLength,  GLsizei *length,  GLchar *infoLog)
{
    return fl_glGetShaderInfoLog(shader, maxLength, length, infoLog);
}

GLuint FlGL::glCreateProgram()
{
    return fl_glCreateProgram();
}

void FlGL::glDeleteProgram(GLuint program)
{
    return fl_glDeleteProgram(program);
}

void FlGL::glAttachShader(GLuint program, GLuint shader)
{
    return fl_glAttachShader(program, shader);
}

void FlGL::glLinkProgram(GLuint program)
{
    return fl_glLinkProgram(program);
}

void FlGL::glGetProgramiv(GLuint program, GLenum pname,  GLint *params)
{
    return fl_glGetProgramiv(program, pname, params);
}

void FlGL::glGetProgramInfoLog(GLuint program,  GLsizei maxLength,
                               GLsizei *length,  GLchar *infoLog)
{
    return fl_glGetProgramInfoLog(program, maxLength, length, infoLog);
}

void FlGL::glViewport(GLint x,  GLint y,  GLsizei width,  GLsizei height)
{
    return fl_glViewport(x, y, width, height);
}

void FlGL::glUseProgram(GLuint program)
{
    return fl_glUseProgram(program);
}

void FlGL::glVertexAttribPointer(GLuint index, GLint size, GLenum type,
                                 GLboolean normalized, GLsizei stride, const GLvoid *pointer)
{
    return fl_glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

void FlGL::glEnableVertexAttribArray(GLuint index)
{
    return fl_glEnableVertexAttribArray(index);
}

void FlGL::glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
    return fl_glDrawArrays(mode, first, count);
}

void FlGL::glClear(GLbitfield mask)
{
    return fl_glClear(mask);
}

GLenum FlGL::glGetError()
{
    return fl_glGetError();
}

void FlGL::glGenTextures(GLsizei n, GLuint *textures)
{
    return fl_glGenTextures(n, textures);
}

void FlGL::glBindTexture(GLenum target,  GLuint texture)
{
    return fl_glBindTexture(target, texture);
}

void FlGL::glTexImage2D(GLenum target, GLint level, GLint internalFormat,
                        GLsizei width, GLsizei height,  GLint border,  GLenum format,
                        GLenum  type, const GLvoid *data)
{
    return fl_glTexImage2D(target, level, internalFormat,
                           width, height, border, format, type, data);
}

void FlGL::glTexParameteri(GLenum target, GLenum  pname, GLint  param)
{
    return fl_glTexParameteri(target, pname, param);
}

void FlGL::glTexParameterf(GLenum  target,  GLenum  pname,  GLfloat  param)
{
    return fl_glTexParameterf(target, pname, param);
}

void FlGL::glUniform1i(GLint location, GLint v0)
{
    return fl_glUniform1i(location, v0);
}

void FlGL::glUniform2i(GLint location, GLint v0, GLint v1)
{
    return fl_glUniform2i(location, v0, v1);
}

void FlGL::glUniform3i(GLint location, GLint v0, GLint v1, GLint v2)
{
    return fl_glUniform3i(location, v0, v1, v2);
}

void FlGL::glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
    return fl_glUniform4i(location, v0, v1, v2, v3);

}

void FlGL::glUniform1f(GLint location, GLfloat v0)
{
    return fl_glUniform1f(location, v0);
}

void FlGL::glUniform2f(GLint location, GLfloat v0, GLfloat v1)
{
    return fl_glUniform2f(location, v0, v1);
}

void FlGL::glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    return fl_glUniform3f(location, v0, v1, v2);
}

void FlGL::glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    return fl_glUniform4f(location, v0, v1, v2, v3);
}

GLint FlGL::glGetAttribLocation(GLuint program,  const GLchar *name)
{
    return fl_glGetAttribLocation(program, name);
}

GLint FlGL::glGetUniformLocation(GLuint program,  const GLchar *name)
{
    return fl_glGetUniformLocation(program, name);
}

void FlGL::glActiveTexture(GLenum  texture)
{
    return fl_glActiveTexture(texture);
}

void FlGL::glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    return fl_glUniformMatrix4fv(location, count, transpose, value);
}

void FlGL::glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    return fl_glClearColor(red, green, blue, alpha);
}

void FlGL::glEnable(GLenum cap)
{
    return fl_glEnable(cap);
}

void FlGL::glBlendFunc(GLenum  sfactor,  GLenum  dfactor)
{
    return fl_glBlendFunc(sfactor, dfactor);
}

void FlGL::glDeleteTextures(GLsizei n, const GLuint *textures)
{
    return fl_glDeleteTextures(n, textures);
}

const GLubyte *FlGL::glGetString(GLenum  name)
{
    return fl_glGetString(name);
}

void FlGL::glCompressedTexImage2D(GLenum target,  GLint level, GLenum internalformat, GLsizei width, GLsizei height,
                                  GLint border, GLsizei imageSize, const GLvoid *data)
{
    return fl_glCompressedTexImage2D(target, level, internalformat,
                                     width, height, border, imageSize, data);
}

void FlGL::glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices)
{
    return fl_glDrawElements(mode, count, type, indices);
}
