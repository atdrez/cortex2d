#include "frameitem.h"


FrameItem::FrameItem(const CtString &texturePath, CtSceneItem *parent)
    : CtSceneImage(parent),
      m_count(0),
      m_frame(0),
      m_frameIndex(0)
{
    CtTexture *texture = CtTextureCache::find(texturePath);
    m_texture = (!texture || !texture->isAtlas()) ? 0 : static_cast<CtAtlasTexture *>(texture);

    CT_ASSERT(!m_texture, "Invalid texture");
    setTexture(m_texture);
}

FrameItem::~FrameItem()
{
    releaseFrameKeys();
}

void FrameItem::releaseFrameKeys()
{
    if (m_frameIndex) {
        delete [] m_frameIndex;
        m_count = 0;
        m_frame = 0;
        m_frameIndex = 0;
    }
}

void FrameItem::setFrame(int index)
{
    int oldIndex = m_frame;

    if (index >= 0 && index < m_count) {
        m_frame = index;
        setTextureAtlasIndex(m_frameIndex[index]);

        if (oldIndex != index)
            frameChange(index);
    }
}

void FrameItem::setFrameKey(const char *key)
{
    setFrameKeys(&key, 1);
}

void FrameItem::setFrameKeys(const char **keys, int count)
{
    CT_ASSERT(!m_texture, "Invalid texture");

    if (count <= 0)
        return;

    releaseFrameKeys();

    m_count = count;
    m_frameIndex = new int[count];

    for (int i = 0; i < count; i++) {
        m_frameIndex[i] = m_texture->indexOfKey(keys[i]);
    }

    setFrame(0);
    CtRect rect = m_texture->viewportRectAt(m_frameIndex[0]);

    if (rect.isValid()) {
        setWidth(rect.width());
        setHeight(rect.height());
    }
}

bool FrameItem::isValidFrame(const char *key) const
{
    return m_texture->indexOfKey(key) >= 0;
}
