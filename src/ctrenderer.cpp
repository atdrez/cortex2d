#include "ctrenderer.h"
#include "ctopenglfunctions.h"

CtRenderer::CtRenderer()
{
    CtGL::glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_defaultFbo);
    m_fbo = m_defaultFbo;
}

void CtRenderer::begin()
{
    CtGL::glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void CtRenderer::end()
{
    // XXX: optimize
    //CtGL::glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CtRenderer::bindBuffer(GLuint id)
{
    m_fbo = id;
    m_buffers.push_back(id);
}

void CtRenderer::releaseBuffer()
{
    if (m_buffers.empty())
        return;

    m_buffers.pop_back();

    if (m_buffers.empty())
        m_fbo = m_defaultFbo;
    else
        m_fbo = m_buffers.back();
}

bool CtRenderer::drawSolid(CtShaderEffect *effect, ctreal width, ctreal height,
                           ctreal r, ctreal g, ctreal b, ctreal a)
{
    if (!effect || !effect->init())
        return false;

    effect->drawSolid(m_projectionMatrix, width, height, r, g, b, a, m_opacity);
    return true;
}

bool CtRenderer::drawTexture(CtShaderEffect *effect, CtTexture *texture,
                             ctreal width, ctreal height)
{
    if (!texture || !texture->isValid() || !effect || !effect->init())
        return false;

    effect->drawTexture(m_projectionMatrix, texture, width, height,
                        m_opacity, false, false, -1);
    return true;
}

bool CtRenderer::drawTexture(CtShaderEffect *effect, CtTexture *texture,
                             ctreal width, ctreal height,
                             bool tileVertically, bool tileHorizontally, int textureAtlasIndex)
{
    if (!texture || !texture->isValid() || !effect || !effect->init())
        return false;

    effect->drawTexture(m_projectionMatrix, texture, width, height, m_opacity,
                        tileVertically, tileHorizontally, textureAtlasIndex);
    return true;
}

bool CtRenderer::drawTexture(CtShaderEffect *effect, CtTexture *texture,
                             GLfloat *vertices, GLfloat *texCoords, int count,
                             bool vTile, bool hTile)
{
    if (!texture || !texture->isValid() || !effect || !effect->init())
        return false;

    effect->drawTexPoly(m_projectionMatrix, texture, vertices, texCoords, count, m_opacity,
                        vTile, hTile);
    return true;
}

bool CtRenderer::drawElements(CtShaderEffect *effect, CtTexture *texture,
                              const CtList<CtShaderEffect::Element> &elements)
{
    if (!texture || !texture->isValid() || !effect || !effect->init())
        return false;

    effect->drawElements(m_projectionMatrix, texture, m_opacity, false, false, elements);
    return true;
}

bool CtRenderer::drawVboTextTexture(CtShaderEffect *effect, CtTexture *texture,
                                    GLuint indexBuffer, GLuint vertexBuffer,
                                    int elementCount, const CtColor &color)
{
    if (!effect || !effect->init())
        return false;

    effect->drawVboTextTexture(m_projectionMatrix, texture, indexBuffer, vertexBuffer,
                               elementCount, color, m_opacity);
    return true;
}
