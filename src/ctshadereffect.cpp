#include "ctshadereffect.h"
#include "ctshaderuniform.h"
#include "ctopenglfunctions.h"
#include "ctfont.h"


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


CtShaderEffect::CtShaderEffect(CtShaderProgram *program)
    : m_ready(false),
      m_program(program),
      m_locColor(-1),
      m_locMatrix(-1),
      m_locOpacity(-1),
      m_locTexture(-1),
      m_locPosition(-1),
      m_locTexCoord(-1)
{
    CT_ASSERT(m_program != 0);
}

CtShaderEffect::~CtShaderEffect()
{

}

void CtShaderEffect::addUniform(CtShaderUniform *uniform)
{
    m_uniforms.removeAll(uniform);
    m_uniforms.append(uniform);
    m_ready = false;
}

void CtShaderEffect::removeUniform(CtShaderUniform *uniform)
{
    m_uniforms.removeAll(uniform);
    m_ready = false;
}

bool CtShaderEffect::init()
{
    if (m_ready)
        return true;

    if (!m_program->link())
        return false;

    m_locMatrix = m_program->uniformLocation("ct_Matrix");
    m_locPosition = m_program->attributeLocation("ct_Vertex");
    m_locOpacity = m_program->uniformLocation("ct_Opacity");
    m_locTexCoord = m_program->attributeLocation("ct_TexCoord");
    m_locTexture = m_program->uniformLocation("ct_Texture0");
    m_locColor = m_program->uniformLocation("ct_Color");

    // update custom uniforms
    foreach (CtShaderUniform *uniform, m_uniforms) {
        uniform->m_location = m_program->uniformLocation(uniform->m_name.data());
    }

    m_ready = true;
    return true;
}

void CtShaderEffect::drawSolid(const CtMatrix4x4 &matrix, ctreal dw, ctreal dh,
                               ctreal r, ctreal g, ctreal b, ctreal a, ctreal opacity)
{
    m_program->bind();

    GLfloat vertices[8];
    ct_setTriangleStripArray(vertices, 0, 0, dw, dh);

    // XXX: do we need to expose this for gradients?
    GLfloat pixCoordinates[8];
    ct_setTriangleStripArray(pixCoordinates, 0, 0, 1, 1);

    applyPosition(matrix, vertices);
    applyColor(r, g, b, a, opacity);
    applyTexCoordinates(pixCoordinates);
    applyCustomUniforms();

    CtGL::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_program->release();
}

void CtShaderEffect::drawPoly(const CtMatrix4x4 &matrix, GLfloat *vertices, int count,
                              ctreal r, ctreal g, ctreal b, ctreal a, ctreal opacity)
{
    m_program->bind();

    applyPosition(matrix, vertices);
    applyColor(r, g, b, a, opacity);
    applyCustomUniforms();

    CtGL::glDrawArrays(GL_LINE_LOOP, 0, count);

    m_program->release();
}

void CtShaderEffect::drawSolidPoly(const CtMatrix4x4 &matrix, GLfloat *vertices, int count,
                                   ctreal r, ctreal g, ctreal b, ctreal a, ctreal opacity)
{
    m_program->bind();

    applyPosition(matrix, vertices);
    applyColor(r, g, b, a, opacity);
    applyCustomUniforms();

    CtGL::glDrawArrays(GL_TRIANGLE_FAN, 0, count);

    m_program->release();
}

void CtShaderEffect::drawTexPoly(const CtMatrix4x4 &matrix, CtTexture *texture,
                                 GLfloat *vertices, GLfloat *texCoords, int count,
                                 ctreal opacity, bool vTile, bool hTile)
{
    if (!texture)
        return;

    m_program->bind();

    applyPosition(matrix, vertices);
    applyColor(1, 1, 1, 1, opacity);
    applyTexture(texture->id(), vTile, hTile);
    applyTexCoordinates(texCoords);
    applyCustomUniforms();

    CtGL::glDrawArrays(GL_TRIANGLE_FAN, 0, count);

    m_program->release();
}

void CtShaderEffect::drawTexture(const CtMatrix4x4 &matrix, CtTexture *texture,
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
        m_program->bind();

        applyPosition(matrix, vertices);
        applyColor(1, 1, 1, 1, opacity);
        applyTexture(texture->id(), vTile, hTile);
        applyTexCoordinates(texCoords);
        applyCustomUniforms();

        CtGL::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        m_program->release();
    }
}

void CtShaderEffect::drawElements(const CtMatrix4x4 &matrix, CtTexture *texture,
                                  ctreal opacity, bool vTile, bool hTile,
                                  const CtList<Element> &elements)
{
    const int count = elements.size();

    if (!texture || count == 0)
        return;

    GLfloat vopacity[count * 6];
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

            const int bo = n * 6;
            vopacity[bo + 0] = e.opacity;
            vopacity[bo + 1] = e.opacity;
            vopacity[bo + 2] = e.opacity;
            vopacity[bo + 3] = e.opacity;
            vopacity[bo + 4] = e.opacity;
            vopacity[bo + 5] = e.opacity;

            n++;
        }
    }

    if (n > 0) {
        m_program->bind();

        int locFragOpacity = m_program->attributeLocation("a_opacity");

        m_program->enableVertexAttributeArray(locFragOpacity);
        m_program->setVertexAttributePointer(locFragOpacity, 1, vopacity);

        applyPosition(matrix, vertices);
        applyColor(1, 1, 1, 1, opacity);
        applyTexture(texture->id(), vTile, hTile);
        applyTexCoordinates(texCoords);
        applyCustomUniforms();


        CtGL::glDrawElements(GL_TRIANGLES, n * 6, GL_UNSIGNED_SHORT, indexes);

        m_program->release();
    }
}

void CtShaderEffect::drawVboTextTexture(const CtMatrix4x4 &matrix, CtTexture *texture,
                                        GLuint indexBuffer, GLuint vertexBuffer,
                                        int elementCount, const CtColor &color, ctreal opacity)
{
    m_program->bind();

    CtGL::glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    m_program->setUniformValue(m_locMatrix, matrix);

    const int vertexStructSize = 4 * sizeof(GLfloat);

    if (m_locPosition >= 0) {
        m_program->setVertexAttributePointer(m_locPosition, GL_FLOAT, 2, vertexStructSize,
                                             reinterpret_cast<void *>(0));
        m_program->enableVertexAttributeArray(m_locPosition);
    }

    if (m_locTexCoord >= 0) {
        m_program->setVertexAttributePointer(m_locTexCoord, GL_FLOAT, 2, vertexStructSize,
                                             reinterpret_cast<void *>(2 * sizeof(GLfloat)));
        m_program->enableVertexAttributeArray(m_locTexCoord);
    }

    applyTexture(texture->id(), false, false);
    applyColor(color.red(), color.green(), color.blue(), color.alpha(), opacity);
    applyCustomUniforms();

    CtGL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    CtGL::glDrawElements(GL_TRIANGLES, elementCount * 6,
                         GL_UNSIGNED_SHORT, reinterpret_cast<void *>(0));

    CtGL::glBindBuffer(GL_ARRAY_BUFFER, 0);
    CtGL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_program->release();
}


void CtShaderEffect::applyPosition(const CtMatrix4x4 &matrix, const GLfloat *vertices)
{
    // XXX: HANDLE Z-ORDER
    m_program->setUniformValue(m_locMatrix, matrix);

    if (m_locPosition >= 0) {
        m_program->setVertexAttributePointer(m_locPosition, 2, vertices);
        m_program->enableVertexAttributeArray(m_locPosition);
    }
}

void CtShaderEffect::applyTexCoordinates(const GLfloat *coords)
{
    if (m_locTexCoord >= 0) {
        m_program->setVertexAttributePointer(m_locTexCoord, 2, coords);
        m_program->enableVertexAttributeArray(m_locTexCoord);
    }
}

void CtShaderEffect::applyColor(ctreal r, ctreal g, ctreal b, ctreal a, ctreal opacity)
{
    m_program->setUniformValue(m_locOpacity, opacity);
    m_program->setUniformValue(m_locColor, r, g, b, a);
}

void CtShaderEffect::applyTexture(GLint textureId, bool vTile, bool hTile)
{
    if (m_locTexture >= 0 && textureId >= 0) {
        // bind texture
        CtGL::glActiveTexture(GL_TEXTURE0);
        CtGL::glBindTexture(GL_TEXTURE_2D, textureId);

        // use texture unit 0 (GL_TEXTURE0)
        m_program->setUniformValue(m_locTexture, int(0));

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
            m_program->setUniformValue(u->m_location, u->m.ivalue);
            break;
        case CtShaderUniform::FloatType:
            m_program->setUniformValue(u->m_location, u->m.rvalue);
            break;
        case CtShaderUniform::Vec2Type:
            m_program->setUniformValue(u->m_location, u->m.v2f.r1, u->m.v2f.r2);
            break;
        case CtShaderUniform::Vec4Type:
            m_program->setUniformValue(u->m_location, u->m.v4f.r1, u->m.v4f.r2, u->m.v4f.r3, u->m.v4f.r4);
            break;
        case CtShaderUniform::Texture1Type:
            CtGL::glActiveTexture(GL_TEXTURE1);
            CtGL::glBindTexture(GL_TEXTURE_2D, u->m.ivalue);
            m_program->setUniformValue(u->m_location, int(1));
            break;
        case CtShaderUniform::Texture2Type:
            CtGL::glActiveTexture(GL_TEXTURE2);
            CtGL::glBindTexture(GL_TEXTURE_2D, u->m.ivalue);
            m_program->setUniformValue(u->m_location, int(2));
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
