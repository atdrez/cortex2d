#include "ctshaderprogram.h"


static const char ct_solidVertexShader[] = " \
    uniform mediump mat4 ct_Matrix;          \
    attribute mediump vec2 ct_Vertex;        \
    attribute mediump vec2 ct_TexCoord;      \
    varying mediump vec2 ct_TexCoord0;       \
                                             \
    void main()                              \
    {                                                        \
        ct_TexCoord0 = ct_TexCoord;                          \
        gl_Position = ct_Matrix * vec4(ct_Vertex, 1.0, 1.0); \
    }                                                        \
   ";

static const char ct_solidFragmentShader[] = " \
    uniform mediump vec4 ct_Color;             \
    uniform mediump float ct_Opacity;          \
    varying mediump vec2 ct_TexCoord0;         \
                                               \
    void main()                                \
    {                                          \
        gl_FragColor = ct_Color;               \
        gl_FragColor.a *= ct_Opacity;          \
    }                                          \
    ";


static const char ct_textureVertexShader[] = " \
    uniform mediump mat4 ct_Matrix;            \
    attribute mediump vec2 ct_Vertex;          \
    attribute mediump vec2 ct_TexCoord;        \
    varying mediump vec2 ct_TexCoord0;         \
                                               \
    void main()                                \
    {                                          \
        gl_PointSize = 1.0;                                  \
        ct_TexCoord0 = ct_TexCoord;                          \
        gl_Position = ct_Matrix * vec4(ct_Vertex, 1.0, 1.0); \
    }                                                        \
   ";

static const char ct_textureFragmentShader[] = " \
    uniform sampler2D ct_Texture0;               \
    uniform mediump float ct_Opacity;            \
    varying mediump vec2 ct_TexCoord0;           \
                                                 \
    void main()                                                       \
    {                                                                 \
        gl_FragColor = texture2D(ct_Texture0, ct_TexCoord0);          \
        gl_FragColor.a *= ct_Opacity;                                 \
    }                                                                 \
    ";


static const char ct_particleVertexShader[] = "   \
    uniform mediump mat4 ct_Matrix;               \
    attribute mediump vec2 ct_Vertex;             \
    attribute mediump vec4 a_color;               \
    attribute mediump float a_size;               \
    varying mediump vec4 v_color;                 \
                                                  \
    void main()                                              \
    {                                                        \
        v_color = a_color;                                   \
        gl_PointSize = a_size;                               \
        gl_Position = ct_Matrix * vec4(ct_Vertex, 1.0, 1.0); \
        gl_Position.w = 1.0;                                 \
    }                                                        \
   ";

static const char ct_particleFragmentShader[] = " \
    #version 120\n                                \
    uniform sampler2D ct_Texture0;                \
    uniform mediump float ct_Opacity;             \
    varying mediump vec4 v_color;                 \
                                                  \
    void main()                                                         \
    {                                                                   \
        gl_FragColor = texture2D(ct_Texture0, gl_PointCoord) * v_color; \
        gl_FragColor.a *= ct_Opacity;                                   \
    }                                                                   \
    ";


static const char ct_textFragmentShader[] = " \
    uniform sampler2D ct_Texture0;            \
    uniform mediump vec4 ct_Color;            \
    uniform mediump float ct_Opacity;         \
    varying mediump vec2 ct_TexCoord0;        \
                                                                        \
    void main()                                                         \
    {                                                                   \
        mediump float alpha = texture2D(ct_Texture0, ct_TexCoord0).a;   \
        gl_FragColor = vec4(ct_Color.rgb, alpha) * ct_Opacity;          \
    }                                                                   \
    ";



CtShaderProgram *CtShaderProgram::sharedSolidShaderProgram()
{
    static CtShaderProgram *result = 0;

    if (!result) {
        result = new CtShaderProgram();
        result->link(ct_solidVertexShader, ct_solidFragmentShader);
    }

    return result;
}

CtShaderProgram *CtShaderProgram::sharedTextureShaderProgram()
{
    static CtShaderProgram *result = 0;

    if (!result) {
        result = new CtShaderProgram();
        result->link(ct_textureVertexShader, ct_textureFragmentShader);
    }

    return result;
}

CtShaderProgram *CtShaderProgram::sharedTextShaderProgram()
{
    static CtShaderProgram *result = 0;

    if (!result) {
        result = new CtShaderProgram();
        result->link(ct_textureVertexShader, ct_textFragmentShader);
    }

    return result;
}

CtShaderProgram *CtShaderProgram::sharedParticleShaderProgram()
{
    static CtShaderProgram *result = 0;

    if (!result) {
        result = new CtShaderProgram();
        result->link(ct_particleVertexShader, ct_particleFragmentShader);
    }

    return result;
}
