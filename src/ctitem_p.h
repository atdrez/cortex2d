#ifndef CTITEM_P_H
#define CTITEM_P_H

#include "ctitem.h"
#include "ctglobal.h"
#include "ctmatrix.h"
#include "ctshadereffect.h"
#include "ctshaderprogram.h"
#include "ctopenglfunctions.h"
#include "ctGL.h"

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

    void checkTransformMatrix();
    CtMatrix currentLocalTransformMatrix();
    CtMatrix currentSceneTransformMatrix();
    CtMatrix currentViewProjectionMatrix();

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
    CtSceneItem *parent;
    CtSceneView *scene;
    ctreal xCenter;
    ctreal yCenter;
    bool transformDirty;
    int flags;
    bool pendingDelete;
    CtMatrix localTransformMatrix;
    CtMatrix sceneTransformMatrix;
    CtList<CtSceneItem *> children;
};

struct CtSceneRectPrivate : public CtSceneItemPrivate
{
    CtSceneRectPrivate(CtSceneRect *q);

    void init(CtSceneItem *parent);
    void release();
    void draw();

    ctreal r;
    ctreal g;
    ctreal b;

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

    void draw();

    CtSceneImage::FillMode fillMode;
};

struct CtSceneFragmentsPrivate : public CtSceneTextureItemPrivate
{
    CtSceneFragmentsPrivate(CtSceneFragments *q);

    void draw();
    void release();

    CtList<CtSceneFragments::Fragment *> fragments;
};

#endif
