#include "flshadereffect.h"
#include "flshaderuniform.h"
#include "flopenglfunctions.h"

static const char fl_textureVertexShader[] = " \
    uniform mediump mat4 fl_Matrix; \
    attribute mediump vec2 fl_Vertex; \
    attribute mediump vec2 fl_TexCoord; \
    varying mediump vec2 fl_TexCoord0; \
    \
    void main() \
    { \
        fl_TexCoord0 = fl_TexCoord; \
        gl_Position = fl_Matrix * vec4(fl_Vertex, 1.0, 1.0); \
        gl_PointSize = 1.0; \
    } \
   ";

static const char fl_textureFragmentShader[] = " \
    uniform sampler2D fl_Texture0; \
    uniform mediump float fl_Opacity; \
    varying mediump vec2 fl_TexCoord0; \
    \
    void main()                                                       \
    {                                                                 \
        gl_FragColor = texture2D(fl_Texture0, fl_TexCoord0);          \
        gl_FragColor.a *= fl_Opacity;                                 \
    }                                                                 \
    ";

static const char fl_solidVertexShader[] = " \
    uniform mediump mat4 fl_Matrix; \
    attribute mediump vec2 fl_Vertex; \
    \
    void main() \
    { \
        gl_Position = fl_Matrix * vec4(fl_Vertex, 1.0, 1.0); \
    } \
   ";

static const char fl_solidFragmentShader[] = " \
    uniform mediump vec4 fl_Color; \
    uniform mediump float fl_Opacity; \
    \
    void main() \
    { \
        gl_FragColor = fl_Color; \
        gl_FragColor.a *= fl_Opacity; \
    } \
    ";


static bool fl_updateVertexAttributes(flreal x, flreal y, flreal w, flreal h,
                                      FlTexture *texture, int atlasIndex, bool vTile, bool hTile,
                                      GLfloat *vertices, GLfloat *texCoords, bool triangleStrip);

static inline void fl_setTriangleArray(GLfloat *v, flreal x1, flreal y1, flreal x2, flreal y2)
{
    v[0] = x1; v[1] = y1; v[2] = x2; v[3] = y1; v[4] = x1;
    v[5] = y2; v[6] = x1; v[7] = y2; v[8] = x2; v[9] = y1;
    v[10] = x2; v[11] = y2;
}

static inline void fl_setTriangleStripArray(GLfloat *v, flreal x1, flreal y1, flreal x2, flreal y2)
{
    v[0] = x1; v[1] = y1; v[2] = x2; v[3] = y1;
    v[4] = x1; v[5] = y2; v[6] = x2; v[7] = y2;
}


FlShaderEffect::FlShaderEffect(Type type)
    : m_type(type),
      m_ready(false),
      m_locMatrix(-1),
      m_locOpacity(-1),
      m_locTexture(-1),
      m_locPosition(-1),
      m_locTexCoord(-1)
{
    if (type == Solid) {
        m_vertexShader = fl_solidVertexShader;
        m_fragmentShader = fl_solidFragmentShader;
    } else {
        m_vertexShader = fl_textureVertexShader;
        m_fragmentShader = fl_textureFragmentShader;
    }
}

FlShaderEffect::~FlShaderEffect()
{

}

FlString FlShaderEffect::vertexShader() const
{
    return m_vertexShader;
}

void FlShaderEffect::setVertexShader(const FlString &shader)
{
    if (m_vertexShader == shader)
        return;

    m_ready = false;
    m_vertexShader = shader;
}

FlString FlShaderEffect::fragmentShader() const
{
    return m_vertexShader;
}

void FlShaderEffect::setFragmentShader(const FlString &shader)
{
    if (m_fragmentShader == shader)
        return;

    m_ready = false;
    m_fragmentShader = shader;
}

void FlShaderEffect::addUniform(FlShaderUniform *uniform)
{
    m_uniforms.remove(uniform);
    m_uniforms.push_back(uniform);
    m_ready = false;
}

void FlShaderEffect::removeUniform(FlShaderUniform *uniform)
{
    m_uniforms.remove(uniform);
    m_ready = false;
}

bool FlShaderEffect::init()
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

    m_locMatrix = FlGL::glGetUniformLocation(m_program.id(), "fl_Matrix");
    m_locPosition = FlGL::glGetAttribLocation(m_program.id(), "fl_Vertex");
    m_locOpacity = FlGL::glGetUniformLocation(m_program.id(), "fl_Opacity");

    if (m_type == Texture) {
        m_locTexCoord = FlGL::glGetAttribLocation(m_program.id(), "fl_TexCoord");
        m_locTexture = FlGL::glGetUniformLocation(m_program.id(), "fl_Texture0");
    } else if (m_type == Solid) {
        m_locColor = FlGL::glGetUniformLocation(m_program.id(), "fl_Color");
    }

    // update custom uniforms
    foreach (FlShaderUniform *uniform, m_uniforms) {
        uniform->m_location = FlGL::glGetUniformLocation(m_program.id(),
                                                         uniform->m_name.c_str());
    }

    m_ready = true;
    return true;
}

void FlShaderEffect::drawSolid(const FlMatrix &matrix, flreal dw, flreal dh,
                               flreal r, flreal g, flreal b, flreal a, flreal opacity)
{
    m_program.bind();

    GLfloat vertices[8];
    fl_setTriangleStripArray(vertices, 0, 0, dw, dh);

    applyPosition((GLfloat *)matrix.data(), vertices);
    applyColor(r, g, b, a, opacity);
    applyCustomUniforms();

    FlGL::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_program.release();
}

void FlShaderEffect::drawTexture(const FlMatrix &matrix, FlTexture *texture,
                                 flreal width, flreal height, flreal opacity,
                                 bool vTile, bool hTile, int textureAtlasIndex)
{
    if (!texture)
        return;

    GLfloat vertices[8];
    GLfloat texCoords[8];
    bool ok = fl_updateVertexAttributes(0, 0, width, height, texture, textureAtlasIndex,
                                        vTile, hTile, vertices, texCoords, true);

    if (ok) {
        m_program.bind();

        applyPosition((GLfloat *)matrix.data(), vertices);
        applyColor(1, 1, 1, 1, opacity);
        applyTexture(texCoords, texture->id(), vTile, hTile);
        applyCustomUniforms();

        FlGL::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        m_program.release();
    }
}

void FlShaderEffect::drawElements(const FlMatrix &matrix, FlTexture *texture,
                                  flreal opacity, bool vTile, bool hTile,
                                  const FlList<Element> &elements)
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
        bool ok = fl_updateVertexAttributes(e.x, e.y, e.width, e.height,
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
        applyTexture(texCoords, texture->id(), vTile, hTile);
        applyCustomUniforms();

        FlGL::glDrawElements(GL_TRIANGLES, n * 6, GL_UNSIGNED_SHORT, indexes);

        m_program.release();
    }
}

void FlShaderEffect::applyPosition(const GLfloat *matrix, const GLfloat *vertices)
{
    // XXX: HANDLE Z-ORDER

    if (m_locMatrix >= 0)
        FlGL::glUniformMatrix4fv(m_locMatrix, 1, GL_FALSE, matrix);

    if (m_locPosition >= 0) {
        FlGL::glVertexAttribPointer(m_locPosition, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        FlGL::glEnableVertexAttribArray(m_locPosition);
    }
}

void FlShaderEffect::applyColor(flreal r, flreal g, flreal b, flreal a, flreal opacity)
{
    if (m_locOpacity >= 0)
        FlGL::glUniform1f(m_locOpacity, opacity);

    if (m_locColor >= 0)
        FlGL::glUniform4f(m_locColor, r, g, b, a);
}

void FlShaderEffect::applyTexture(const GLfloat *texCoords, GLint textureId, bool vTile, bool hTile)
{
    if (m_locTexCoord >= 0 && texCoords) {
        FlGL::glVertexAttribPointer(m_locTexCoord, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
        FlGL::glEnableVertexAttribArray(m_locTexCoord);
    }

    if (m_locTexture >= 0 && textureId >= 0) {
        // bind texture
        FlGL::glActiveTexture(GL_TEXTURE0);
        FlGL::glBindTexture(GL_TEXTURE_2D, textureId);

        // use texture unit 0 (GL_TEXTURE0)
        FlGL::glUniform1i(m_locTexture, 0);

        // fill mode
        FlGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                              hTile ? GL_REPEAT : GL_CLAMP_TO_EDGE);
        FlGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                              vTile ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    }
}

void FlShaderEffect::applyCustomUniforms()
{
    foreach (FlShaderUniform *u, m_uniforms) {
        // skip invalid uniforms
        if (u->m_location < 0)
            continue;

        switch (u->type()) {
        case FlShaderUniform::IntType:
            FlGL::glUniform1i(u->m_location, u->m.ivalue);
            break;
        case FlShaderUniform::FloatType:
            FlGL::glUniform1f(u->m_location, u->m.rvalue);
            break;
        case FlShaderUniform::Vec2Type:
            FlGL::glUniform2f(u->m_location, u->m.vec2.r1, u->m.vec2.r2);
            break;
        default:
            break;
        }
    }
}

bool fl_updateVertexAttributes(flreal x, flreal y, flreal w, flreal h,
                               FlTexture *texture, int atlasIndex, bool vTile, bool hTile,
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
        const FlAtlasTexture *atlas = static_cast<FlAtlasTexture *>(texture);
        const FlRect &sourceRect = atlas->sourceRectAt(atlasIndex);
        const FlRect &viewRect = atlas->viewportRectAt(atlasIndex);

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

            dx += w * flreal(-viewRect.x()) / vw;
            dy += h * flreal(-viewRect.y()) / vh;
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
        fl_setTriangleStripArray(vertices, dx1, dy1, dx2, dy2);
        fl_setTriangleStripArray(texCoords, tx1, ty1, tx2, ty2);
    } else {
        fl_setTriangleArray(vertices, dx1, dy1, dx2, dy2);
        fl_setTriangleArray(texCoords, tx1, ty1, tx2, ty2);
    }

    return true;
}
