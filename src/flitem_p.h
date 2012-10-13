#ifndef FLITEM_P_H
#define FLITEM_P_H

#include "flitem.h"
#include "flglobal.h"
#include "flmatrix.h"
#include "flshadereffect.h"
#include "flshaderprogram.h"
#include "flopenglfunctions.h"
#include "flGL.h"

struct FlSceneItemPrivate
{
    FlSceneItemPrivate(FlSceneItem *q);

    virtual void init(FlSceneItem *parent);
    virtual void release();

    void addItem(FlSceneItem *item);
    void removeItem(FlSceneItem *item);
    void setScene(FlSceneView *newScene);

    void checkTransformMatrix();
    FlMatrix currentLocalTransformMatrix();
    FlMatrix currentSceneTransformMatrix();
    FlMatrix currentViewProjectionMatrix();

    FlSceneItem *q;
    flreal x;
    flreal y;
    flreal z;
    flreal width;
    flreal height;
    flreal xScale;
    flreal yScale;
    flreal rotation;
    flreal opacity;
    bool visible;
    FlSceneItem *parent;
    FlSceneView *scene;
    flreal xCenter;
    flreal yCenter;
    bool transformDirty;
    int flags;
    FlMatrix localTransformMatrix;
    FlMatrix sceneTransformMatrix;
    FlList<FlSceneItem *> children;
};

struct FlSceneRectPrivate : public FlSceneItemPrivate
{
    FlSceneRectPrivate(FlSceneRect *q);

    void init(FlSceneItem *parent);
    void release();
    void draw();

    flreal r;
    flreal g;
    flreal b;

    FlShaderEffect *shaderEffect;
};


struct FlSceneImagePrivate : public FlSceneItemPrivate
{
    FlSceneImagePrivate(FlSceneImage *q);

    void init(FlSceneItem *parent);
    void release();
    void draw();

    int textureAtlasIndex;
    bool ownTexture;
    FlTexture *texture;
    FlShaderEffect *shaderEffect;
    FlSceneImage::FillMode fillMode;
};

#endif
