#ifndef CTITEM_P_H
#define CTITEM_P_H

#include "ctitem.h"
#include "ctglobal.h"
#include "ctmatrix.h"
#include "ctshadereffect.h"
#include "ctshaderprogram.h"
#include "ctopenglfunctions.h"
#include "ctGL.h"
#include "ctmath.h"
#include "ctfont.h"
#include "ctcolor.h"

class CtRenderer;

struct CtSpritePrivate
{
    CtSpritePrivate(CtSprite *q);
    virtual ~CtSpritePrivate();

    virtual void init(CtSprite *parent);
    virtual void release();

    void addItem(CtSprite *item);
    void removeItem(CtSprite *item);
    void setScene(CtSceneView *newScene);

    bool relativeVisible();
    ctreal relativeOpacity();
    bool relativeFrozen();
    CtSceneFrameBuffer *frameBufferItem();

    void checkTransformMatrix();
    CtMatrix mappedTransformMatrix(CtSprite *root);

    CtMatrix currentLocalTransformMatrix();
    CtMatrix currentSceneTransformMatrix();
    CtMatrix4x4 currentViewportProjectionMatrix();

    void fillItems(CtList<CtSprite *> &lst);
    virtual void recursivePaint(CtRenderer *state);

    const CtList<CtSprite *> &orderedChildren();

    CtSprite *q;
    ctreal x;
    ctreal y;
    ctreal z;
    ctreal width;
    ctreal height;
    ctreal xScale;
    ctreal yScale;
    ctreal rotation;
    ctreal opacity;
    bool visible;
    bool isFrozen;
    ctreal implicitWidth;
    ctreal implicitHeight;
    CtSprite *parent;
    CtSceneView *scene;
    ctreal xCenter;
    ctreal yCenter;
    bool sortDirty;
    bool transformDirty;
    int flags;
    bool isFrameBuffer;
    bool pendingDelete;
    CtMatrix fboTransformMatrix;
    CtMatrix localMatrix;
    CtMatrix localTransformMatrix;
    CtMatrix sceneTransformMatrix;

    CtList<CtSprite *> children;
    CtList<CtSprite *> sortedChildren;
};

struct CtSceneRectPrivate : public CtSpritePrivate
{
    CtSceneRectPrivate(CtSceneRect *q);

    void init(CtSprite *parent);
    void release();

    CtColor color;
    CtShaderEffect *shaderEffect;
};

struct CtSceneTextPrivate : public CtSpritePrivate
{
    CtSceneTextPrivate(CtSceneText *q);

    void init(CtSprite *parent);
    void release();

    void releaseBuffers();
    void recreateBuffers();

    CtColor color;
    CtString text;
    int glyphCount;
    GLuint indexBuffer;
    GLuint vertexBuffer;
    CtFont *font;
    CtShaderEffect *shaderEffect;
};

struct CtSceneFrameBufferPrivate : public CtSpritePrivate
{
    CtSceneFrameBufferPrivate(CtSceneFrameBuffer *q);

    void init(CtSprite *parent);
    void release();

    void deleteBuffers();
    void resizeBuffer(int w, int h);

    virtual void recursivePaint(CtRenderer *state);

    int bufferWidth;
    int bufferHeight;
    GLuint framebuffer;
    GLuint depthbuffer;
    CtTexture *texture;
    CtShaderEffect *shaderEffect;
};

struct CtSceneTextureItemPrivate : public CtSpritePrivate
{
    CtSceneTextureItemPrivate(CtSceneTextureItem *q);

    void init(CtSprite *parent);
    void release();

    int textureAtlasIndex;
    bool ownTexture;
    CtTexture *texture;
    CtShaderEffect *shaderEffect;
};

struct CtSceneImagePrivate : public CtSceneTextureItemPrivate
{
    CtSceneImagePrivate(CtSceneImage *q);

    CtSceneImage::FillMode fillMode;
};

struct CtSceneImagePolyPrivate : public CtSceneImagePrivate
{
    CtSceneImagePolyPrivate(CtSceneImagePoly *q);

    CtVector<CtPoint> vertices;
};

struct CtSceneFragmentsPrivate : public CtSceneTextureItemPrivate
{
    CtSceneFragmentsPrivate(CtSceneFragments *q);

    void init(CtSprite *parent);
    void release();

    CtList<CtSceneFragments::Fragment *> fragments;
};

struct CtSceneParticleSystemPrivate : public CtSceneTextureItemPrivate
{
    CtSceneParticleSystemPrivate(CtSceneParticleSystem *q);

    void init(CtSprite *parent);
    void release();

    void recreateVertexBuffer();

    GLfloat *vertices;
    int attrCount;
    int vertexSize;
    int vertexCount;
    CtVector<CtSceneParticleSystem::Particle *> particles;
};

#endif
