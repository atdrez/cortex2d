#ifndef CTOPENGLFUNCTIONS_H
#define CTOPENGLFUNCTIONS_H

#include "ctglobal.h"
#include "ctGL.h"

#ifndef CT_GL_APIENTRY
#    if defined(APIENTRY)
#        define CT_GL_APIENTRY APIENTRY
#    else
#        define CT_GL_APIENTRY
#    endif
#endif

#ifndef CT_GL_APIENTRYP
#  define CT_GL_APIENTRYP CT_GL_APIENTRY *
#endif

class CtGL
{
public:
    typedef void *(CT_GL_APIENTRYP Func_GetProcAddress)(const char *func);

    static GLuint glCreateShader(GLenum  shaderType);
    static void glDeleteShader(GLenum  shaderType);
    static void glShaderSource(GLuint shader,  GLsizei count, const GLchar **string, const GLint *length);
    static void glCompileShader(GLenum  shaderType);
    static void glGetShaderiv(GLuint shader,  GLenum pname,  GLint *params);
    static void glGetShaderInfoLog(GLuint shader,  GLsizei maxLength,  GLsizei *length,  GLchar *infoLog);
    static GLuint glCreateProgram(void);
    static void glDeleteProgram(GLuint program);
    static void glAttachShader(GLuint program,  GLuint shader);
    static void glLinkProgram(GLuint program);
    static void glGetProgramiv(GLuint program,  GLenum pname,  GLint *params);
    static void glGetProgramInfoLog(GLuint program,  GLsizei maxLength,  GLsizei *length,  GLchar *infoLog);
    static void glViewport(GLint x,  GLint y,  GLsizei width,  GLsizei height);
    static void glUseProgram(GLuint program);
    static void glVertexAttribPointer(GLuint index,  GLint size,  GLenum type,  GLboolean normalized,  GLsizei stride,  const GLvoid * pointer);
    static void glEnableVertexAttribArray(GLuint index);
    static void glDrawArrays(GLenum mode,  GLint first,  GLsizei count);
    static void glClear(GLbitfield mask);
    static GLenum glGetError();
    static void glGenTextures(GLsizei n, GLuint *textures);
    static void glBindTexture(GLenum target,  GLuint texture);
    static void glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height,  GLint border,  GLenum format, GLenum  type, const GLvoid *  data);
    static void glTexParameteri(GLenum  target,  GLenum  pname,  GLint  param);
    static void glTexParameterf(GLenum  target,  GLenum  pname,  GLfloat  param);
    static void glUniform1i(GLint location, GLint v0);
    static void glUniform2i(GLint location, GLint v0, GLint v1);
    static void glUniform3i(GLint location, GLint v0, GLint v1, GLint v2);
    static void glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
    static void glUniform1f(GLint location, GLfloat v0);
    static void glUniform2f(GLint location, GLfloat v0, GLfloat v1);
    static void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
    static void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
    static GLint glGetAttribLocation(GLuint program,  const GLchar *name);
    static GLint glGetUniformLocation(GLuint program,  const GLchar *name);
    static void glActiveTexture(GLenum  texture);
    static void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    static void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
    static void glEnable(GLenum cap);
    static void glBlendFunc(GLenum  sfactor, GLenum  dfactor);
    static void glDeleteTextures(GLsizei n, const GLuint *textures);
    static const GLubyte *glGetString(GLenum  name);
    static void glCompressedTexImage2D(GLenum target,  GLint level, GLenum internalformat, GLsizei width, GLsizei height,
                                       GLint border, GLsizei imageSize, const GLvoid *data);

    static void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices);

    static CtString getGlErrorMessage(GLenum error);
    static GLuint loadTGATexture(const char *fileName);
    static void assignFunctions(Func_GetProcAddress func);

    static void resetGlError();

    static void glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
    static void glGenFramebuffers(GLsizei n, GLuint *ids);
    static void glGenRenderbuffers(GLsizei n, GLuint *renderbuffers);
    static void glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
    static void glBindFramebuffer(GLenum target, GLuint framebuffer);
    static void glBindRenderbuffer(GLenum target, GLuint renderbuffer);
    static GLenum glCheckFramebufferStatus(GLenum target);

    static void glGenBuffers(GLsizei  n,  GLuint *  buffers);

    static void glBindBuffer(GLenum  target,  GLuint  buffer);
    static void glBufferData(GLenum  target,  GLsizeiptr  size,  const GLvoid *  data,  GLenum  usage);
    static void glGetIntegerv(GLenum pname, GLint *params);

    static void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);

    static void glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers);
    static void glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers);
    static void glDeleteBuffers(GLsizei n,  const GLuint *buffers);

    static void glGetRenderbufferParameteriv(GLenum target,  GLenum pname,  GLint * params);
};

#endif
