#ifndef CTSHADEREFFECT_H
#define CTSHADEREFFECT_H

#include "ctmatrix.h"
#include "cttexture.h"
#include "ctshaderprogram.h"
#include <map>

class CtRenderer;
class CtShaderUniform;

class CtShaderEffect
{
public:
    enum Type {
        Solid,
        Texture
    };

    struct Element {
        ctreal x;
        ctreal y;
        ctreal width;
        ctreal height;
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

    CtShaderEffect(Type type = Texture);
    virtual ~CtShaderEffect();

    CtString vertexShader() const;
    void setVertexShader(const CtString &shader);

    CtString fragmentShader() const;
    void setFragmentShader(const CtString &shader);

    void addUniform(CtShaderUniform *uniform);
    void removeUniform(CtShaderUniform *uniform);

    bool init();

    void drawPoly(const CtMatrix &matrix, GLfloat *vertices, int count,
                  ctreal r, ctreal g, ctreal b, ctreal a, ctreal opacity);

    void drawSolidPoly(const CtMatrix &matrix, GLfloat *vertices, int count,
                       ctreal r, ctreal g, ctreal b, ctreal a, ctreal opacity);

    void drawTexPoly(const CtMatrix &matrix, CtTexture *texture,
                     GLfloat *vertices, GLfloat *texCoords, int count,
                     ctreal opacity, int textureAtlasIndex);

protected:
    void applyPosition(const GLfloat *matrix, const GLfloat *vertices);
    void applyTexCoordinates(const GLfloat *coords);
    void applyColor(ctreal r, ctreal g, ctreal b, ctreal a, ctreal opacity);
    void applyTexture(GLint textureId, bool vTile, bool hTile);
    void applyCustomUniforms();

    void drawSolid(const CtMatrix &matrix, ctreal width, ctreal height,
                   ctreal r, ctreal g, ctreal b, ctreal a, ctreal opacity);

    void drawTexture(const CtMatrix &matrix, CtTexture *texture,
                     ctreal width, ctreal height, ctreal opacity,
                     bool tileVertically, bool tileHorizontally, int textureAtlasIndex);

    void drawElements(const CtMatrix &matrix, CtTexture *texture,
                      ctreal opacity,  bool tileVertically, bool tileHorizontally,
                      const CtList<Element> &elements);

private:
    Type m_type;
    bool m_ready;
    CtString m_vertexShader;
    CtString m_fragmentShader;
    CtShaderProgram m_program;
    GLint m_locColor;
    GLint m_locMatrix;
    GLint m_locOpacity;
    GLint m_locTexture;
    GLint m_locPosition;
    GLint m_locTexCoord;
    CtList<CtShaderUniform *> m_uniforms;

    friend class CtRenderer;
};

#endif
