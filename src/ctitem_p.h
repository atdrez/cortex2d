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


struct CtSceneImagePrivate : public CtSceneItemPrivate
{
    CtSceneImagePrivate(CtSceneImage *q);

    void init(CtSceneItem *parent);
    void release();
    virtual void draw();

    int textureAtlasIndex;
    bool ownTexture;
    CtTexture *texture;
    CtShaderEffect *shaderEffect;
    CtSceneImage::FillMode fillMode;
};

struct CtSceneFragmentsPrivate : public CtSceneImagePrivate
{
    CtSceneFragmentsPrivate(CtSceneImage *q);

    void release();
    void draw();

    CtList<CtSceneFragments::Fragment *> fragments;
};

#endif
