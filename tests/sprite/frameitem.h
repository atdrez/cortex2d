#ifndef FRAMEITEM_H
#define FRAMEITEM_H

#include <Cortex2D>

class FrameItem : public CtImageSprite
{
public:
    FrameItem(const CtString &texturePath, CtSprite *parent = 0);
    ~FrameItem();

    int frame() const { return m_frame; }
    void setFrame(int index);

    int frameCount() const { return m_count; }

    void setFrameKey(const char *key);
    void setFrameKeys(const char **keys, int count);

    bool isValidFrame(const char *key) const;

protected:
    void releaseFrameKeys();
    virtual void frameChange(int) {}

private:
    int m_count;
    int m_frame;
    int *m_frameIndex;
    CtAtlasTexture *m_texture;
};

#endif
