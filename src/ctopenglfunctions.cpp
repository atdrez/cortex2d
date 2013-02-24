#include "ctopenglfunctions.h"

static char *CtGL_loadTGA(char *fileName, int *width, int *height)
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

GLuint CtGL::loadTGATexture(const char *fileName)
{
    int w, h;
    GLuint texture;
    char *buffer = CtGL_loadTGA((char *)fileName, &w, &h);

    if (!buffer) {
        CT_WARNING("Error loading image '" << fileName << "'");
        return 0;
    }

    CtGL::glGenTextures(1, &texture);
    CtGL::glBindTexture(GL_TEXTURE_2D, texture);
    CtGL::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);

    CtGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    CtGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    CtGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    CtGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    delete buffer;

    return texture;
}

void CtGL::resetGlError()
{
    while (CtGL::glGetError() != GL_NO_ERROR);
}

CtString CtGL::getGlErrorMessage(GLenum error)
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

#define CT_GL_FPTR(ret, name, params)                            \
    typedef ret (CT_GL_APIENTRYP Func_##name)params;             \
    static Func_##name ct_##name = name;

CT_GL_FPTR(GLuint, glCreateShader, (GLenum  shaderType))
CT_GL_FPTR(void, glDeleteShader, (GLenum  shaderType))
CT_GL_FPTR(void, glShaderSource, (GLuint shader, GLsizei count, const GLchar **string, const GLint *length))
CT_GL_FPTR(void, glCompileShader, (GLenum  shaderType))
CT_GL_FPTR(void, glGetShaderiv, (GLuint shader,  GLenum pname,  GLint *params))
CT_GL_FPTR(void, glGetShaderInfoLog, (GLuint shader,  GLsizei maxLength,  GLsizei *length,  GLchar *infoLog))
CT_GL_FPTR(GLuint, glCreateProgram, (void))
CT_GL_FPTR(void, glDeleteProgram, (GLuint program))
CT_GL_FPTR(void, glAttachShader, (GLuint program,  GLuint shader))
CT_GL_FPTR(void, glLinkProgram, (GLuint program))
CT_GL_FPTR(void, glGetProgramiv, (GLuint program,  GLenum pname,  GLint *params))
CT_GL_FPTR(void, glGetProgramInfoLog, (GLuint program,  GLsizei maxLength,  GLsizei *length,  GLchar *infoLog))
CT_GL_FPTR(void, glViewport, (GLint x,  GLint y,  GLsizei width,  GLsizei height))
CT_GL_FPTR(void, glUseProgram, (GLuint program))
CT_GL_FPTR(void, glVertexAttribPointer, (GLuint index,  GLint size,  GLenum type,  GLboolean normalized,
                                         GLsizei stride,  const GLvoid * pointer))
CT_GL_FPTR(void, glEnableVertexAttribArray, (GLuint index))
CT_GL_FPTR(void, glDrawArrays, (GLenum mode,  GLint first,  GLsizei count))
CT_GL_FPTR(void, glClear, (GLbitfield mask))
CT_GL_FPTR(GLenum, glGetError, (void))
CT_GL_FPTR(void, glGenTextures, (GLsizei n, GLuint *textures))
CT_GL_FPTR(void, glBindTexture, (GLenum target,  GLuint texture))
CT_GL_FPTR(void, glTexImage2D, (GLenum target, GLint level, GLint internalFormat,
                                GLsizei width, GLsizei height,  GLint border,  GLenum format,
                                GLenum  type, const GLvoid *  data))
CT_GL_FPTR(void, glTexParameteri, (GLenum  target,  GLenum  pname,  GLint  param))
CT_GL_FPTR(void, glTexParameterf, (GLenum  target,  GLenum  pname,  GLfloat  param))

CT_GL_FPTR(void, glUniform1i, (GLint location, GLint v0))
CT_GL_FPTR(void, glUniform2i, (GLint location, GLint v0, GLint v1))
CT_GL_FPTR(void, glUniform3i, (GLint location, GLint v0, GLint v1, GLint v2))
CT_GL_FPTR(void, glUniform4i, (GLint location, GLint v0, GLint v1, GLint v2, GLint v3))

CT_GL_FPTR(void, glUniform1f, (GLint location, GLfloat v0))
CT_GL_FPTR(void, glUniform2f, (GLint location, GLfloat v0, GLfloat v1))
CT_GL_FPTR(void, glUniform3f, (GLint location, GLfloat v0, GLfloat v1, GLfloat v2))
CT_GL_FPTR(void, glUniform4f, (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3))

CT_GL_FPTR(GLint, glGetAttribLocation, (GLuint program,  const GLchar *name))
CT_GL_FPTR(GLint, glGetUniformLocation, (GLuint program,  const GLchar *name))
CT_GL_FPTR(void, glActiveTexture, (GLenum  texture))
CT_GL_FPTR(void, glUniformMatrix4fv, (GLint  location,  GLsizei  count,  GLboolean  transpose,  const GLfloat * value))
CT_GL_FPTR(void, glClearColor, (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha))
CT_GL_FPTR(void, glEnable, (GLenum cap))
CT_GL_FPTR(void, glBlendFunc, (GLenum  sfactor,  GLenum  dfactor))
CT_GL_FPTR(void, glDeleteTextures, (GLsizei n, const GLuint *textures))
CT_GL_FPTR(const GLubyte *, glGetString, (GLenum  name))
CT_GL_FPTR(void, glCompressedTexImage2D, (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize,  const GLvoid *data))
CT_GL_FPTR(void, glDrawElements, (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices))

CT_GL_FPTR(void, glRenderbufferStorage, (GLenum target, GLenum internalformat, GLsizei width, GLsizei height))
CT_GL_FPTR(void, glGenFramebuffers, (GLsizei n, GLuint *ids))
CT_GL_FPTR(void, glGenRenderbuffers, (GLsizei n, GLuint *renderbuffers))
CT_GL_FPTR(void, glFramebufferRenderbuffer, (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer))
CT_GL_FPTR(void, glBindFramebuffer, (GLenum target, GLuint framebuffer))
CT_GL_FPTR(void, glBindRenderbuffer, (GLenum target, GLuint renderbuffer))
CT_GL_FPTR(GLenum, glCheckFramebufferStatus, (GLenum target))
CT_GL_FPTR(void, glFramebufferTexture2D, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level))
CT_GL_FPTR(void, glGetIntegerv, (GLenum pname, GLint *params))
CT_GL_FPTR(void, glBindBuffer, (GLenum  target,  GLuint  buffer))
CT_GL_FPTR(void, glBufferData, (GLenum  target,  GLsizeiptr  size,  const GLvoid *  data,  GLenum  usage))
CT_GL_FPTR(void, glGenBuffers, (GLsizei  n,  GLuint *  buffers))

#undef CT_GL_FPTR

void CtGL::assignFunctions(Func_GetProcAddress f)
{
// We don't need to change these pointers for Android
#ifndef CT_ANDROID

    if (!f)
        return;

#   define CT_GL_FASSIGN(name)                              \
    ct_##name = (Func_##name)f(#name);                      \
    if (!ct_##name) CT_FATAL("Unable to load " #name " function");

    CT_GL_FASSIGN(glCreateShader);
    CT_GL_FASSIGN(glDeleteShader);
    CT_GL_FASSIGN(glShaderSource);
    CT_GL_FASSIGN(glCompileShader);
    CT_GL_FASSIGN(glGetShaderiv);
    CT_GL_FASSIGN(glGetShaderInfoLog);
    CT_GL_FASSIGN(glCreateProgram);
    CT_GL_FASSIGN(glDeleteProgram);
    CT_GL_FASSIGN(glAttachShader);
    CT_GL_FASSIGN(glLinkProgram);
    CT_GL_FASSIGN(glGetProgramiv);
    CT_GL_FASSIGN(glGetProgramInfoLog);
    CT_GL_FASSIGN(glViewport);
    CT_GL_FASSIGN(glUseProgram);
    CT_GL_FASSIGN(glVertexAttribPointer);
    CT_GL_FASSIGN(glEnableVertexAttribArray);
    CT_GL_FASSIGN(glDrawArrays);
    CT_GL_FASSIGN(glClear);
    CT_GL_FASSIGN(glGetError);
    CT_GL_FASSIGN(glGenTextures);
    CT_GL_FASSIGN(glBindTexture);
    CT_GL_FASSIGN(glTexImage2D);
    CT_GL_FASSIGN(glTexParameteri);
    CT_GL_FASSIGN(glTexParameterf);
    CT_GL_FASSIGN(glUniform1i);
    CT_GL_FASSIGN(glUniform2i);
    CT_GL_FASSIGN(glUniform3i);
    CT_GL_FASSIGN(glUniform4i);
    CT_GL_FASSIGN(glUniform1f);
    CT_GL_FASSIGN(glUniform2f);
    CT_GL_FASSIGN(glUniform3f);
    CT_GL_FASSIGN(glUniform4f);
    CT_GL_FASSIGN(glGetAttribLocation);
    CT_GL_FASSIGN(glGetUniformLocation);
    CT_GL_FASSIGN(glActiveTexture);
    CT_GL_FASSIGN(glUniformMatrix4fv);
    CT_GL_FASSIGN(glClearColor);
    CT_GL_FASSIGN(glEnable);
    CT_GL_FASSIGN(glBlendFunc);
    CT_GL_FASSIGN(glDeleteTextures);
    CT_GL_FASSIGN(glGetString);
    CT_GL_FASSIGN(glCompressedTexImage2D);
    CT_GL_FASSIGN(glDrawElements);
    CT_GL_FASSIGN(glRenderbufferStorage);
    CT_GL_FASSIGN(glGenFramebuffers);
    CT_GL_FASSIGN(glGenRenderbuffers);
    CT_GL_FASSIGN(glFramebufferRenderbuffer);
    CT_GL_FASSIGN(glBindFramebuffer);
    CT_GL_FASSIGN(glBindRenderbuffer);
    CT_GL_FASSIGN(glCheckFramebufferStatus);
    CT_GL_FASSIGN(glFramebufferTexture2D);
    CT_GL_FASSIGN(glGetIntegerv);
    CT_GL_FASSIGN(glBindBuffer);
    CT_GL_FASSIGN(glBufferData);
    CT_GL_FASSIGN(glGenBuffers);

#   undef CT_GL_FASSIGN

#endif // CT_ANDROID
}


GLuint CtGL::glCreateShader(GLenum shaderType)
{
    return ct_glCreateShader(shaderType);
}

void CtGL::glDeleteShader(GLenum shaderType)
{
    return ct_glDeleteShader(shaderType);
}

void CtGL::glShaderSource(GLuint shader,  GLsizei count, const GLchar **string, const GLint *length)
{
    return ct_glShaderSource(shader, count, string, length);
}

void CtGL::glCompileShader(GLenum shaderType)
{
    return ct_glCompileShader(shaderType);
}

void CtGL::glGetShaderiv(GLuint shader,  GLenum pname,  GLint *params)
{
    return ct_glGetShaderiv(shader, pname, params);
}

void CtGL::glGetShaderInfoLog(GLuint shader,  GLsizei maxLength,  GLsizei *length,  GLchar *infoLog)
{
    return ct_glGetShaderInfoLog(shader, maxLength, length, infoLog);
}

GLuint CtGL::glCreateProgram()
{
    return ct_glCreateProgram();
}

void CtGL::glDeleteProgram(GLuint program)
{
    return ct_glDeleteProgram(program);
}

void CtGL::glAttachShader(GLuint program, GLuint shader)
{
    return ct_glAttachShader(program, shader);
}

void CtGL::glLinkProgram(GLuint program)
{
    return ct_glLinkProgram(program);
}

void CtGL::glGetProgramiv(GLuint program, GLenum pname,  GLint *params)
{
    return ct_glGetProgramiv(program, pname, params);
}

void CtGL::glGetProgramInfoLog(GLuint program,  GLsizei maxLength,
                               GLsizei *length,  GLchar *infoLog)
{
    return ct_glGetProgramInfoLog(program, maxLength, length, infoLog);
}

void CtGL::glViewport(GLint x,  GLint y,  GLsizei width,  GLsizei height)
{
    return ct_glViewport(x, y, width, height);
}

void CtGL::glUseProgram(GLuint program)
{
    return ct_glUseProgram(program);
}

void CtGL::glVertexAttribPointer(GLuint index, GLint size, GLenum type,
                                 GLboolean normalized, GLsizei stride, const GLvoid *pointer)
{
    return ct_glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

void CtGL::glEnableVertexAttribArray(GLuint index)
{
    return ct_glEnableVertexAttribArray(index);
}

void CtGL::glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
    return ct_glDrawArrays(mode, first, count);
}

void CtGL::glClear(GLbitfield mask)
{
    return ct_glClear(mask);
}

GLenum CtGL::glGetError()
{
    return ct_glGetError();
}

void CtGL::glGenTextures(GLsizei n, GLuint *textures)
{
    return ct_glGenTextures(n, textures);
}

void CtGL::glBindTexture(GLenum target,  GLuint texture)
{
    return ct_glBindTexture(target, texture);
}

void CtGL::glTexImage2D(GLenum target, GLint level, GLint internalFormat,
                        GLsizei width, GLsizei height,  GLint border,  GLenum format,
                        GLenum  type, const GLvoid *data)
{
    return ct_glTexImage2D(target, level, internalFormat,
                           width, height, border, format, type, data);
}

void CtGL::glTexParameteri(GLenum target, GLenum  pname, GLint  param)
{
    return ct_glTexParameteri(target, pname, param);
}

void CtGL::glTexParameterf(GLenum  target,  GLenum  pname,  GLfloat  param)
{
    return ct_glTexParameterf(target, pname, param);
}

void CtGL::glUniform1i(GLint location, GLint v0)
{
    return ct_glUniform1i(location, v0);
}

void CtGL::glUniform2i(GLint location, GLint v0, GLint v1)
{
    return ct_glUniform2i(location, v0, v1);
}

void CtGL::glUniform3i(GLint location, GLint v0, GLint v1, GLint v2)
{
    return ct_glUniform3i(location, v0, v1, v2);
}

void CtGL::glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
    return ct_glUniform4i(location, v0, v1, v2, v3);

}

void CtGL::glUniform1f(GLint location, GLfloat v0)
{
    return ct_glUniform1f(location, v0);
}

void CtGL::glUniform2f(GLint location, GLfloat v0, GLfloat v1)
{
    return ct_glUniform2f(location, v0, v1);
}

void CtGL::glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    return ct_glUniform3f(location, v0, v1, v2);
}

void CtGL::glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    return ct_glUniform4f(location, v0, v1, v2, v3);
}

GLint CtGL::glGetAttribLocation(GLuint program,  const GLchar *name)
{
    return ct_glGetAttribLocation(program, name);
}

GLint CtGL::glGetUniformLocation(GLuint program,  const GLchar *name)
{
    return ct_glGetUniformLocation(program, name);
}

void CtGL::glActiveTexture(GLenum  texture)
{
    return ct_glActiveTexture(texture);
}

void CtGL::glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    return ct_glUniformMatrix4fv(location, count, transpose, value);
}

void CtGL::glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    return ct_glClearColor(red, green, blue, alpha);
}

void CtGL::glEnable(GLenum cap)
{
    return ct_glEnable(cap);
}

void CtGL::glBlendFunc(GLenum  sfactor,  GLenum  dfactor)
{
    return ct_glBlendFunc(sfactor, dfactor);
}

void CtGL::glDeleteTextures(GLsizei n, const GLuint *textures)
{
    return ct_glDeleteTextures(n, textures);
}

const GLubyte *CtGL::glGetString(GLenum  name)
{
    return ct_glGetString(name);
}

void CtGL::glCompressedTexImage2D(GLenum target,  GLint level, GLenum internalformat, GLsizei width, GLsizei height,
                                  GLint border, GLsizei imageSize, const GLvoid *data)
{
    return ct_glCompressedTexImage2D(target, level, internalformat,
                                     width, height, border, imageSize, data);
}

void CtGL::glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices)
{
    return ct_glDrawElements(mode, count, type, indices);
}

void CtGL::glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
    return ct_glRenderbufferStorage(target, internalformat, width, height);
}

void CtGL::glGenFramebuffers(GLsizei n, GLuint *ids)
{
    return ct_glGenFramebuffers(n, ids);
}

void CtGL::glGenRenderbuffers(GLsizei n, GLuint *renderbuffers)
{
    return ct_glGenRenderbuffers(n, renderbuffers);
}

void CtGL::glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
    return ct_glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
}

void CtGL::glBindFramebuffer(GLenum target, GLuint framebuffer)
{
    return ct_glBindFramebuffer(target, framebuffer);
}

void CtGL::glBindRenderbuffer(GLenum target, GLuint renderbuffer)
{
    return ct_glBindRenderbuffer(target, renderbuffer);
}

GLenum CtGL::glCheckFramebufferStatus(GLenum target)
{
    return ct_glCheckFramebufferStatus(target);
}

void CtGL::glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    return ct_glFramebufferTexture2D(target, attachment, textarget, texture, level);
}

void CtGL::glGetIntegerv(GLenum pname, GLint *params)
{
    return ct_glGetIntegerv(pname, params);
}

void CtGL::glBindBuffer(GLenum  target,  GLuint  buffer)
{
    return ct_glBindBuffer(target, buffer);
}

void CtGL::glBufferData(GLenum  target,  GLsizeiptr  size,  const GLvoid *  data,  GLenum  usage)
{
    return ct_glBufferData(target, size, data, usage);
}

void CtGL::glGenBuffers(GLsizei  n,  GLuint *  buffers)
{
    return ct_glGenBuffers(n, buffers);
}
