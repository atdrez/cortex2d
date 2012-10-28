#ifndef FLSHADEREFFECT_H
#define FLSHADEREFFECT_H

#include "flmatrix.h"
#include "fltexture.h"
#include "flshaderprogram.h"
#include <map>

class FlShaderUniform;

class FlShaderEffect
{
public:
    enum Type {
        Solid,
        Texture
    };

    struct Element {
        flreal x;
        flreal y;
        flreal width;
        flreal height;
        int textureAtlasIndex;

        Element()
            : x(0),
              y(0),
              width(0),
              height(0),
              textureAtlasIndex(-1)
        {
        }

        Element(const Element &e)
            : x(e.x),
              y(e.y),
              width(e.width),
              height(e.height),
              textureAtlasIndex(e.textureAtlasIndex)
        {
        }
    };

    FlShaderEffect(Type type = Texture);
    virtual ~FlShaderEffect();

    FlString vertexShader() const;
    void setVertexShader(const FlString &shader);

    FlString fragmentShader() const;
    void setFragmentShader(const FlString &shader);

    void addUniform(FlShaderUniform *uniform);
    void removeUniform(FlShaderUniform *uniform);

protected:
    bool init();
    void applyPosition(const GLfloat *matrix, const GLfloat *vertices);
    void applyColor(flreal r, flreal g, flreal b, flreal a, flreal opacity);
    void applyTexture(const GLfloat *texCoords, GLint textureId, bool vTile, bool hTile);
    void applyCustomUniforms();

    void drawSolid(const FlMatrix &matrix, flreal width, flreal height,
                   flreal r, flreal g, flreal b, flreal a, flreal opacity);

    void drawTexture(const FlMatrix &matrix, FlTexture *texture,
                     flreal width, flreal height, flreal opacity,
                     bool tileVertically, bool tileHorizontally, int textureAtlasIndex);

    void drawElements(const FlMatrix &matrix, FlTexture *texture,
                      flreal opacity,  bool tileVertically, bool tileHorizontally,
                      const FlList<Element> &elements);

private:
    Type m_type;
    bool m_ready;
    FlString m_vertexShader;
    FlString m_fragmentShader;
    FlShaderProgram m_program;
    GLint m_locColor;
    GLint m_locMatrix;
    GLint m_locOpacity;
    GLint m_locTexture;
    GLint m_locPosition;
    GLint m_locTexCoord;
    FlList<FlShaderUniform *> m_uniforms;

    friend class FlSceneImage;
    friend class FlSceneImagePrivate;
    friend class FlSceneRectPrivate;
};

#endif
