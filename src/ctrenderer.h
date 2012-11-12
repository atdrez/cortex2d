#ifndef CTRENDERER_H
#define CTRENDERER_H

#include "ctwindow.h"
#include "ctmatrix.h"
#include "ctshadereffect.h"

class CtTexture;
class CtSceneItemPrivate;
class CtSceneFrameBuffer;
class CtSceneFrameBufferPrivate;

class CtRenderer
{
public:
    CtRenderer();

    void bindBuffer(GLuint id);
    void releaseBuffer();

    ctreal opacity() const { return m_opacity; }

    CtMatrix projectionMatrix() const { return m_projectionMatrix; }

    bool drawSolid(CtShaderEffect *effect, ctreal width, ctreal height,
                   ctreal r, ctreal g, ctreal b, ctreal a);

    bool drawTexture(CtShaderEffect *effect, CtTexture *texture,
                     ctreal width, ctreal height);

    bool drawTexture(CtShaderEffect *effect, CtTexture *texture,
                     ctreal width, ctreal height,
                     bool tileVertically, bool tileHorizontally, int textureAtlasIndex);

    bool drawElements(CtShaderEffect *effect, CtTexture *texture,
                      const CtList<CtShaderEffect::Element> &elements);

private:
    void begin();
    void end();

    friend class CtSceneItemPrivate;
    friend class CtSceneFrameBufferPrivate;

    GLint m_fbo;
    GLint m_defaultFbo;
    ctreal m_opacity;
    CtList<GLuint> m_buffers;
    CtMatrix m_projectionMatrix;
};

#endif
