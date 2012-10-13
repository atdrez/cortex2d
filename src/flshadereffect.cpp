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

    flreal dx = 0;
    flreal dy = 0;

    const GLfloat vertices[8] = {
        dx, dy, (dx + dw), dy, dx,
        (dy + dh), (dx + dw), (dy + dh)
    };

    if (m_locOpacity >= 0)
        FlGL::glUniform1f(m_locOpacity, opacity);

    if (m_locColor >= 0)
        FlGL::glUniform4f(m_locColor, r, g, b, a);

    if (m_locMatrix >= 0)
        FlGL::glUniformMatrix4fv(m_locMatrix, 1, GL_FALSE, (GLfloat *)matrix.data());

    if (m_locPosition >= 0) {
        FlGL::glVertexAttribPointer(m_locPosition, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        FlGL::glEnableVertexAttribArray(m_locPosition);
    }

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

    m_program.bind();

    GLfloat tw = texture->width();
    GLfloat th = texture->height();
    GLfloat sx = 0, sy = 0, sw = tw, sh = th;
    GLfloat dx = 0, dy = 0, dw = width, dh = height;

    if (texture->isAtlas() && textureAtlasIndex >= 0) {
        FlAtlasTexture *atlas = static_cast<FlAtlasTexture *>(texture);

        FlRect sourceRect = atlas->sourceRectAt(textureAtlasIndex);
        if (sourceRect.isValid()) {
            sx = sourceRect.x();
            sy = sourceRect.y();
            sw = sourceRect.width();
            sh = sourceRect.height();
        }

        FlRect viewRect = atlas->viewportRectAt(textureAtlasIndex);
        if (viewRect.isValid()) {
            dx = width * flreal(-viewRect.x()) / viewRect.width();
            dy = height * flreal(-viewRect.y()) / viewRect.height();
            dw = width * sw / viewRect.width();
            dh = height * sh / viewRect.height();
        }
    }

    GLfloat texXFactor = hTile ? width / tw : 1.0f;
    GLfloat texYFactor = vTile ? height / th : 1.0f;
    GLfloat texY1 = !texture->isInverted() ? (sy / th) : (1.0 - (sy / th));
    GLfloat texY2 = !texture->isInverted() ? ((sy + sh) / th) : (1.0 - ((sy + sh) / th));

    const GLfloat vertices[8] = {
        dx, dy, (dx + dw), dy, dx,
        (dy + dh), (dx + dw), (dy + dh)
    };

    const GLfloat texCoords[8] = {
        (sx / tw), texY1 * texYFactor, ((sx + sw) / tw) * texXFactor, texY1 * texYFactor,
        (sx / tw), texY2 * texYFactor, ((sx + sw) / tw) * texXFactor, texY2 * texYFactor
    };

    // XXX: HANDLE Z-ORDER

    if (m_locOpacity >= 0)
        FlGL::glUniform1f(m_locOpacity, opacity);

    if (m_locMatrix >= 0)
        FlGL::glUniformMatrix4fv(m_locMatrix, 1, GL_FALSE, (GLfloat *)matrix.data());

    if (m_locPosition >= 0) {
        FlGL::glVertexAttribPointer(m_locPosition, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        FlGL::glEnableVertexAttribArray(m_locPosition);
    }

    if (m_locTexCoord >= 0) {
        FlGL::glVertexAttribPointer(m_locTexCoord, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
        FlGL::glEnableVertexAttribArray(m_locTexCoord);
    }

    if (m_locTexture >= 0) {
        // bind texture
        FlGL::glActiveTexture(GL_TEXTURE0);
        FlGL::glBindTexture(GL_TEXTURE_2D, texture->id());

        // use texture unit 0 (GL_TEXTURE0)
        FlGL::glUniform1i(m_locTexture, 0);

        // fill mode
        FlGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                              hTile ? GL_REPEAT : GL_CLAMP_TO_EDGE);
        FlGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                              vTile ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    }

    applyCustomUniforms();

    FlGL::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    m_program.release();
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
