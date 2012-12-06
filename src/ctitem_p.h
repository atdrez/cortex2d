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

struct CtSceneItemPrivate
{
    CtSceneItemPrivate(CtSceneItem *q);
    virtual ~CtSceneItemPrivate();

    virtual void init(CtSceneItem *parent);
    virtual void release();

    void addItem(CtSceneItem *item);
    void removeItem(CtSceneItem *item);
    void setScene(CtSceneView *newScene);

    bool relativeVisible();
    ctreal relativeOpacity();
    bool relativeFrozen();
    CtSceneFrameBuffer *frameBufferItem();

    void checkTransformMatrix();
    CtMatrix mappedTransformMatrix(CtSceneItem *root);

    CtMatrix currentLocalTransformMatrix();
    CtMatrix currentSceneTransformMatrix();
    CtMatrix currentViewportProjectionMatrix();

    void fillItems(CtList<CtSceneItem *> &lst);
    virtual void recursivePaint(CtRenderer *state);

    const CtList<CtSceneItem *> &orderedChildren();

    CtSceneItem *q;
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
    CtSceneItem *parent;
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

    CtList<CtSceneItem *> children;
    CtList<CtSceneItem *> sortedChildren;
};

struct CtSceneRectPrivate : public CtSceneItemPrivate
{
    CtSceneRectPrivate(CtSceneRect *q);

    void init(CtSceneItem *parent);
    void release();

    ctreal r;
    ctreal g;
    ctreal b;

    CtShaderEffect *shaderEffect;
};

struct CtSceneTextPrivate : public CtSceneItemPrivate
{
    CtSceneTextPrivate(CtSceneText *q);

    void init(CtSceneItem *parent);
    void release();

    void releaseBuffers();
    void recreateBuffers();

    CtColor color;
    CtString text;
    int glyphCount;
    GLuint indexBuffer;
    GLuint vertexBuffer;
    CtTextureFont *font;
    CtShaderEffect *shaderEffect;
};

struct CtSceneFrameBufferPrivate : public CtSceneItemPrivate
{
    CtSceneFrameBufferPrivate(CtSceneFrameBuffer *q);

    void init(CtSceneItem *parent);
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

struct CtSceneTextureItemPrivate : public CtSceneItemPrivate
{
    CtSceneTextureItemPrivate(CtSceneTextureItem *q);

    void init(CtSceneItem *parent);
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

struct CtSceneFragmentsPrivate : public CtSceneTextureItemPrivate
{
    CtSceneFragmentsPrivate(CtSceneFragments *q);

    void release();

    CtList<CtSceneFragments::Fragment *> fragments;
};

#endif
