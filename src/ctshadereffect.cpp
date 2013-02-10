#include "ctshadereffect.h"
#include "ctshaderuniform.h"
#include "ctopenglfunctions.h"
#include "ctfont.h"
#include "ctfont_p.h"
#include "ctfontmanager_p.h"


static const char ct_textureVertexShader[] = " \
    uniform mediump mat4 ct_Matrix; \
    attribute mediump vec2 ct_Vertex; \
    attribute mediump vec2 ct_TexCoord; \
    varying mediump vec2 ct_TexCoord0; \
    \
    void main() \
    { \
        ct_TexCoord0 = ct_TexCoord; \
        gl_Position = ct_Matrix * vec4(ct_Vertex, 1.0, 1.0); \
        gl_PointSize = 1.0; \
    } \
   ";

static const char ct_textureFragmentShader[] = " \
    uniform sampler2D ct_Texture0; \
    uniform mediump float ct_Opacity; \
    varying mediump vec2 ct_TexCoord0; \
    \
    void main()                                                       \
    {                                                                 \
        gl_FragColor = texture2D(ct_Texture0, ct_TexCoord0);          \
        gl_FragColor.a *= ct_Opacity;                                 \
    }                                                                 \
    ";

static const char ct_solidVertexShader[] = " \
    uniform mediump mat4 ct_Matrix; \
    attribute mediump vec2 ct_Vertex; \
    attribute mediump vec2 ct_TexCoord; \
    varying mediump vec2 ct_TexCoord0; \
    \
    void main() \
    { \
        ct_TexCoord0 = ct_TexCoord;                          \
        gl_Position = ct_Matrix * vec4(ct_Vertex, 1.0, 1.0); \
    } \
   ";

static const char ct_solidFragmentShader[] = " \
    uniform mediump vec4 ct_Color; \
    uniform mediump float ct_Opacity; \
    varying mediump vec2 ct_TexCoord0; \
    \
    void main() \
    { \
        gl_FragColor = ct_Color; \
        gl_FragColor.a *= ct_Opacity; \
    } \
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


static bool ct_updateVertexAttributes(ctreal x, ctreal y, ctreal w, ctreal h,
                                      CtTexture *texture, int atlasIndex, bool vTile, bool hTile,
                                      GLfloat *vertices, GLfloat *texCoords, bool triangleStrip);

static inline void ct_setTriangleArray(GLfloat *v, ctreal x1, ctreal y1, ctreal x2, ctreal y2)
{
    v[0] = x1; v[1] = y1; v[2] = x2; v[3] = y1; v[4] = x1;
    v[5] = y2; v[6] = x1; v[7] = y2; v[8] = x2; v[9] = y1;
    v[10] = x2; v[11] = y2;
}

static inline void ct_setTriangleStripArray(GLfloat *v, ctreal x1, ctreal y1, ctreal x2, ctreal y2)
{
    v[0] = x1; v[1] = y1; v[2] = x2; v[3] = y1;
    v[4] = x1; v[5] = y2; v[6] = x2; v[7] = y2;
}


CtShaderEffect::CtShaderEffect(Type type)
    : m_type(type),
      m_ready(false),
      m_locMatrix(-1),
      m_locOpacity(-1),
      m_locTexture(-1),
      m_locPosition(-1),
      m_locTexCoord(-1)
{
    switch (type) {
    case Solid:
        m_vertexShader = ct_solidVertexShader;
        m_fragmentShader = ct_solidFragmentShader;
        break;
    case Texture:
        m_vertexShader = ct_textureVertexShader;
        m_fragmentShader = ct_textureFragmentShader;
        break;
    case TextureText:
        m_vertexShader = ct_textureVertexShader;
        m_fragmentShader = ct_textFragmentShader;
        break;
    default:
        break;
    }
}

CtShaderEffect::~CtShaderEffect()
{

}

CtString CtShaderEffect::vertexShader() const
{
    return m_vertexShader;
}

void CtShaderEffect::setVertexShader(const CtString &shader)
{
    if (m_vertexShader == shader)
        return;

    m_ready = false;
    m_vertexShader = shader;
}

CtString CtShaderEffect::fragmentShader() const
{
    return m_vertexShader;
}

void CtShaderEffect::setFragmentShader(const CtString &shader)
{
    if (m_fragmentShader == shader)
        return;

    m_ready = false;
    m_fragmentShader = shader;
}

void CtShaderEffect::addUniform(CtShaderUniform *uniform)
{
    m_uniforms.remove(uniform);
    m_uniforms.push_back(uniform);
    m_ready = false;
}

void CtShaderEffect::removeUniform(CtShaderUniform *uniform)
{
    m_uniforms.remove(uniform);
    m_ready = false;
}

bool CtShaderEffect::init()
{
    if (m_ready)
        return true;

    if (m_vertexShader.empty() || m_fragmentShader.empty())
        return false;

    // XXX: share shader if source code is the same
    m_program.unlink();

    m_program.loadVertexShader(m_vertexShader);
    m_program.loadFragmentShader(m_fragmentShader);

    if (!m_program.link())
        return false;

    m_program.releaseVertexShader();
    m_program.releaseFragmentShader();

    m_locMatrix = CtGL::glGetUniformLocation(m_program.id(), "ct_Matrix");
    m_locPosition = CtGL::glGetAttribLocation(m_program.id(), "ct_Vertex");
    m_locOpacity = CtGL::glGetUniformLocation(m_program.id(), "ct_Opacity");

    if (m_type == Texture) {
        m_locTexCoord = CtGL::glGetAttribLocation(m_program.id(), "ct_TexCoord");
        m_locTexture = CtGL::glGetUniformLocation(m_program.id(), "ct_Texture0");
    } else if (m_type == Solid) {
        m_locColor = CtGL::glGetUniformLocation(m_program.id(), "ct_Color");
        m_locTexCoord = CtGL::glGetAttribLocation(m_program.id(), "ct_TexCoord");
    } else if (m_type == TextureText) {
        m_locColor = CtGL::glGetUniformLocation(m_program.id(), "ct_Color");
        m_locTexCoord = CtGL::glGetAttribLocation(m_program.id(), "ct_TexCoord");
        m_locTexture = CtGL::glGetUniformLocation(m_program.id(), "ct_Texture0");
    }

    // update custom uniforms
    foreach (CtShaderUniform *uniform, m_uniforms) {
        uniform->m_location = CtGL::glGetUniformLocation(m_program.id(),
                                                         uniform->m_name.c_str());
    }

    m_ready = true;
    return true;
}

void CtShaderEffect::drawSolid(const CtMatrix &matrix, ctreal dw, ctreal dh,
                               ctreal r, ctreal g, ctreal b, ctreal a, ctreal opacity)
{
    m_program.bind();

    GLfloat vertices[8];
    ct_setTriangleStripArray(vertices, 0, 0, dw, dh);

    // XXX: do we need to expose this for gradients?
    GLfloat pixCoordinates[8];
    ct_setTriangleStripArray(pixCoordinates, 0, 0, 1, 1);

    applyPosition((GLfloat *)matrix.data(), vertices);
    applyColor(r, g, b, a, opacity);
    applyTexCoordinates(pixCoordinates);
    applyCustomUniforms();

    CtGL::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_program.release();
}

void CtShaderEffect::drawPoly(const CtMatrix &matrix, GLfloat *vertices, int count,
                              ctreal r, ctreal g, ctreal b, ctreal a, ctreal opacity)
{
    m_program.bind();

    applyPosition((GLfloat *)matrix.data(), vertices);
    applyColor(r, g, b, a, opacity);
    applyCustomUniforms();

    CtGL::glDrawArrays(GL_LINE_LOOP, 0, count);

    m_program.release();
}

void CtShaderEffect::drawSolidPoly(const CtMatrix &matrix, GLfloat *vertices, int count,
                                   ctreal r, ctreal g, ctreal b, ctreal a, ctreal opacity)
{
    m_program.bind();

    applyPosition((GLfloat *)matrix.data(), vertices);
    applyColor(r, g, b, a, opacity);
    applyCustomUniforms();

    CtGL::glDrawArrays(GL_TRIANGLE_FAN, 0, count);

    m_program.release();
}

void CtShaderEffect::drawTexPoly(const CtMatrix &matrix, CtTexture *texture,
                                 GLfloat *vertices, GLfloat *texCoords, int count,
                                 ctreal opacity, bool vTile, bool hTile)
{
    if (!texture)
        return;

    m_program.bind();

    applyPosition((GLfloat *)matrix.data(), vertices);
    applyColor(1, 1, 1, 1, opacity);
    applyTexture(texture->id(), vTile, hTile);
    applyTexCoordinates(texCoords);
    applyCustomUniforms();

    CtGL::glDrawArrays(GL_TRIANGLE_FAN, 0, count);

    m_program.release();
}

void CtShaderEffect::drawTexture(const CtMatrix &matrix, CtTexture *texture,
                                 ctreal width, ctreal height, ctreal opacity,
                                 bool vTile, bool hTile, int textureAtlasIndex)
{
    if (!texture)
        return;

    GLfloat vertices[8];
    GLfloat texCoords[8];
    bool ok = ct_updateVertexAttributes(0, 0, width, height, texture, textureAtlasIndex,
                                        vTile, hTile, vertices, texCoords, true);

    if (ok) {
        m_program.bind();

        applyPosition((GLfloat *)matrix.data(), vertices);
        applyColor(1, 1, 1, 1, opacity);
        applyTexture(texture->id(), vTile, hTile);
        applyTexCoordinates(texCoords);
        applyCustomUniforms();

        CtGL::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        m_program.release();
    }
}

void CtShaderEffect::drawElements(const CtMatrix &matrix, CtTexture *texture,
                                  ctreal opacity, bool vTile, bool hTile,
                                  const CtList<Element> &elements)
{
    const int count = elements.size();

    if (!texture || count == 0)
        return;

    GLfloat vertices[12 * count];
    GLfloat texCoords[12 * count];
    unsigned short indexes[count * 6];

    int n = 0;
    int j = 0;

    foreach (const Element &e, elements) {
        int offset = n * 12;
        bool ok = ct_updateVertexAttributes(e.x, e.y, e.width, e.height,
                                            texture, e.textureAtlasIndex, vTile, hTile,
                                            vertices + offset, texCoords + offset, false);

        if (ok) {
            // indexes
            for (int w = j + 6; j < w; j++)
                indexes[j] = j;
            n++;
        }
    }

    if (n > 0) {
        m_program.bind();

        applyPosition((GLfloat *)matrix.data(), vertices);
        applyColor(1, 1, 1, 1, opacity);
        applyTexture(texture->id(), vTile, hTile);
        applyTexCoordinates(texCoords);
        applyCustomUniforms();

        CtGL::glDrawElements(GL_TRIANGLES, n * 6, GL_UNSIGNED_SHORT, indexes);

        m_program.release();
    }
}

void CtShaderEffect::drawVboTextTexture(const CtMatrix &matrix, CtTexture *texture,
                                        GLuint indexBuffer, GLuint vertexBuffer,
                                        int elementCount, const CtColor &color, ctreal opacity)
{
    m_program.bind();

    CtGL::glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    if (m_locMatrix >= 0)
        CtGL::glUniformMatrix4fv(m_locMatrix, 1, GL_FALSE,
                                 reinterpret_cast<GLfloat *>(matrix.data()));

    const int vertexStructSize = 4 * sizeof(GLfloat);

    if (m_locPosition >= 0) {
        CtGL::glVertexAttribPointer(m_locPosition, 2, GL_FLOAT, GL_FALSE, vertexStructSize,
                                    reinterpret_cast<void *>(0));
        CtGL::glEnableVertexAttribArray(m_locPosition);
    }

    if (m_locTexCoord >= 0) {
        CtGL::glVertexAttribPointer(m_locTexCoord, 2, GL_FLOAT, GL_FALSE, vertexStructSize,
                                    reinterpret_cast<void *>(2 * sizeof(GLfloat)));
        CtGL::glEnableVertexAttribArray(m_locTexCoord);
    }

    applyTexture(texture->id(), false, false);
    applyColor(color.r(), color.g(), color.b(), color.a(), opacity);
    applyCustomUniforms();

    CtGL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    CtGL::glDrawElements(GL_TRIANGLES, elementCount * 6,
                         GL_UNSIGNED_SHORT, reinterpret_cast<void *>(0));

    CtGL::glBindBuffer(GL_ARRAY_BUFFER, 0);
    CtGL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_program.release();
}


void CtShaderEffect::applyPosition(const GLfloat *matrix, const GLfloat *vertices)
{
    // XXX: HANDLE Z-ORDER

    if (m_locMatrix >= 0)
        CtGL::glUniformMatrix4fv(m_locMatrix, 1, GL_FALSE, matrix);

    if (m_locPosition >= 0) {
        CtGL::glVertexAttribPointer(m_locPosition, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        CtGL::glEnableVertexAttribArray(m_locPosition);
    }
}

void CtShaderEffect::applyTexCoordinates(const GLfloat *coords)
{
    if (m_locTexCoord >= 0) {
        CtGL::glVertexAttribPointer(m_locTexCoord, 2, GL_FLOAT, GL_FALSE, 0, coords);
        CtGL::glEnableVertexAttribArray(m_locTexCoord);
    }
}

void CtShaderEffect::applyColor(ctreal r, ctreal g, ctreal b, ctreal a, ctreal opacity)
{
    if (m_locOpacity >= 0)
        CtGL::glUniform1f(m_locOpacity, opacity);

    if (m_locColor >= 0)
        CtGL::glUniform4f(m_locColor, r, g, b, a);
}

void CtShaderEffect::applyTexture(GLint textureId, bool vTile, bool hTile)
{
    if (m_locTexture >= 0 && textureId >= 0) {
        // bind texture
        CtGL::glActiveTexture(GL_TEXTURE0);
        CtGL::glBindTexture(GL_TEXTURE_2D, textureId);

        // use texture unit 0 (GL_TEXTURE0)
        CtGL::glUniform1i(m_locTexture, 0);

        // fill mode
        CtGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                              hTile ? GL_REPEAT : GL_CLAMP_TO_EDGE);
        CtGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                              vTile ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    }
}

void CtShaderEffect::applyCustomUniforms()
{
    foreach (CtShaderUniform *u, m_uniforms) {
        // skip invalid uniforms
        if (u->m_location < 0)
            continue;

        switch (u->type()) {
        case CtShaderUniform::IntType:
            CtGL::glUniform1i(u->m_location, u->m.ivalue);
            break;
        case CtShaderUniform::FloatType:
            CtGL::glUniform1f(u->m_location, u->m.rvalue);
            break;
        case CtShaderUniform::Vec2Type:
            CtGL::glUniform2f(u->m_location, u->m.v2f.r1, u->m.v2f.r2);
            break;
        case CtShaderUniform::Vec4Type:
            CtGL::glUniform4f(u->m_location, u->m.v4f.r1, u->m.v4f.r2, u->m.v4f.r3, u->m.v4f.r4);
            break;
        default:
            break;
        }
    }
}

bool ct_updateVertexAttributes(ctreal x, ctreal y, ctreal w, ctreal h,
                               CtTexture *texture, int atlasIndex, bool vTile, bool hTile,
                               GLfloat *vertices, GLfloat *texCoords, bool triangleStrip)
{
    if (!texture)
        return false;

    const GLfloat tw = texture->width();
    const GLfloat th = texture->height();
    const bool inverted = texture->isInverted();

    if (tw == 0 || th == 0)
        return false;

    GLfloat sx = 0;
    GLfloat sy = 0;
    GLfloat sw = tw;
    GLfloat sh = th;

    GLfloat dx = x;
    GLfloat dy = y;
    GLfloat dw = w;
    GLfloat dh = h;

    if (atlasIndex >= 0 && texture->isAtlas()) {
        const CtAtlasTexture *atlas = static_cast<CtAtlasTexture *>(texture);
        const CtRect &sourceRect = atlas->sourceRectAt(atlasIndex);
        const CtRect &viewRect = atlas->viewportRectAt(atlasIndex);

        if (sourceRect.isValid()) {
            sx = sourceRect.x();
            sy = sourceRect.y();
            sw = sourceRect.width();
            sh = sourceRect.height();
        }

        if (viewRect.isValid()) {
            const GLfloat vw = viewRect.width();
            const GLfloat vh = viewRect.height();

            if (vw == 0 || vh == 0)
                return false;

            dx += w * ctreal(-viewRect.x()) / vw;
            dy += h * ctreal(-viewRect.y()) / vh;
            dw = w * sw / vw;
            dh = h * sh / vh;
        }
    }

    // nothing to draw
    if (sw == 0 || sh == 0 || dw == 0 || dh == 0)
        return false;

    const GLfloat txf = hTile ? (w / tw) : 1.0f;
    const GLfloat tyf = vTile ? (h / th) : 1.0f;
    const GLfloat tty1 = !inverted ? (sy / th) : (1.0 - (sy / th));
    const GLfloat tty2 = !inverted ? ((sy + sh) / th) : (1.0 - ((sy + sh) / th));

    // vertex coords
    const GLfloat dx1 = dx;
    const GLfloat dy1 = dy;
    const GLfloat dx2 = (dx + dw);
    const GLfloat dy2 = (dy + dh);

    // texture coords
    const GLfloat tx1 = (sx / tw);
    const GLfloat ty1 = tty1 * tyf;
    const GLfloat tx2 = ((sx + sw) / tw) * txf;
    const GLfloat ty2 = tty2 * tyf;

    if (triangleStrip) {
        ct_setTriangleStripArray(vertices, dx1, dy1, dx2, dy2);
        ct_setTriangleStripArray(texCoords, tx1, ty1, tx2, ty2);
    } else {
        ct_setTriangleArray(vertices, dx1, dy1, dx2, dy2);
        ct_setTriangleArray(texCoords, tx1, ty1, tx2, ty2);
    }

    return true;
}
