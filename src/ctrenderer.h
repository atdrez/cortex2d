#ifndef CTRENDERER_H
#define CTRENDERER_H

#include "ctwindow.h"
#include "ctmatrix.h"
#include "ctshadereffect.h"

class CtTexture;
class CtSpritePrivate;
class CtFrameBufferSprite;
class CtFrameBufferSpritePrivate;

class CtRenderer
{
public:
    CtRenderer();

    void bindBuffer(GLuint id);
    void releaseBuffer();

    ctreal opacity() const { return m_opacity; }

    CtMatrix4x4 projectionMatrix() const { return m_projectionMatrix; }

    bool drawSolid(CtShaderEffect *effect, ctreal width, ctreal height,
                   ctreal r, ctreal g, ctreal b, ctreal a);

    bool drawTexture(CtShaderEffect *effect, CtTexture *texture,
                     ctreal width, ctreal height);

    bool drawTexture(CtShaderEffect *effect, CtTexture *texture,
                     ctreal width, ctreal height,
                     bool tileVertically, bool tileHorizontally, int textureAtlasIndex);

    bool drawTexture(CtShaderEffect *effect, CtTexture *texture,
                     GLfloat *vertices, GLfloat *texCoords, int count,
                     bool vTile, bool hTile);

    bool drawElements(CtShaderEffect *effect, CtTexture *texture,
                      const CtList<CtShaderEffect::Element> &elements);

    bool drawVboTextTexture(CtShaderEffect *effect, CtTexture *texture,
                            GLuint indexBuffer, GLuint vertexBuffer,
                            int elementCount, const CtColor &color);

private:
    void begin();
    void end();

    friend class CtSprite;
    friend class CtFrameBufferSprite;

    GLint m_fbo;
    GLint m_defaultFbo;
    ctreal m_opacity;
    CtList<GLuint> m_buffers;
    CtMatrix4x4 m_projectionMatrix;

    CT_PRIVATE_COPY(CtRenderer);
};

#endif
