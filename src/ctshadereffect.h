#ifndef CTSHADEREFFECT_H
#define CTSHADEREFFECT_H

#include "ctcolor.h"
#include "ctmatrix.h"
#include "cttexture.h"
#include "ctshaderprogram.h"
#include <map>

class CtRenderer;
class CtTextureFont;
class CtShaderUniform;

class CtShaderEffect
{
public:
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

    CtShaderEffect(CtShaderProgram *program);
    virtual ~CtShaderEffect();

    CtShaderProgram *program() const { return m_program; }

    CtString vertexShader() const;
    void setVertexShader(const CtString &shader);

    CtString fragmentShader() const;
    void setFragmentShader(const CtString &shader);

    void addUniform(CtShaderUniform *uniform);
    void removeUniform(CtShaderUniform *uniform);

    bool init();

    void drawPoly(const CtMatrix4x4 &matrix, GLfloat *vertices, int count,
                  ctreal r, ctreal g, ctreal b, ctreal a, ctreal opacity);

    void drawSolidPoly(const CtMatrix4x4 &matrix, GLfloat *vertices, int count,
                       ctreal r, ctreal g, ctreal b, ctreal a, ctreal opacity);

    void drawTexPoly(const CtMatrix4x4 &matrix, CtTexture *texture,
                     GLfloat *vertices, GLfloat *texCoords, int count,
                     ctreal opacity, bool vTile, bool hTile);

protected:
    void applyPosition(const CtMatrix4x4 &matrix, const GLfloat *vertices);
    void applyTexCoordinates(const GLfloat *coords);
    void applyColor(ctreal r, ctreal g, ctreal b, ctreal a, ctreal opacity);
    void applyTexture(GLint textureId, bool vTile, bool hTile);
    void applyCustomUniforms();

    void drawSolid(const CtMatrix4x4 &matrix, ctreal width, ctreal height,
                   ctreal r, ctreal g, ctreal b, ctreal a, ctreal opacity);

    void drawTexture(const CtMatrix4x4 &matrix, CtTexture *texture,
                     ctreal width, ctreal height, ctreal opacity,
                     bool tileVertically, bool tileHorizontally, int textureAtlasIndex);

    void drawElements(const CtMatrix4x4 &matrix, CtTexture *texture,
                      ctreal opacity,  bool tileVertically, bool tileHorizontally,
                      const CtList<Element> &elements);

    void drawVboTextTexture(const CtMatrix4x4 &matrix, CtTexture *texture,
                            GLuint indexBuffer, GLuint vertexBuffer,
                            int elementCount, const CtColor &color, ctreal opacity);

private:
    bool m_ready;
    CtShaderProgram *m_program;
    GLint m_locColor;
    GLint m_locMatrix;
    GLint m_locOpacity;
    GLint m_locTexture;
    GLint m_locPosition;
    GLint m_locTexCoord;
    CtList<CtShaderUniform *> m_uniforms;

    friend class CtRenderer;
};

static inline void ct_setTriangle2Array(GLfloat *v, ctreal x1, ctreal y1, ctreal x2, ctreal y2,
                                        ctreal s1, ctreal t1, ctreal s2, ctreal t2)
{
    v[0] = x1; v[1] = y1; v[2] = s1; v[3] = t1;
    v[4] = x2; v[5] = y1; v[6] = s2; v[7] = t1;
    v[8] = x1; v[9] = y2; v[10] = s1; v[11] = t2;
    v[12] = x1; v[13] = y2; v[14] = s1; v[15] = t2;
    v[16] = x2; v[17] = y1; v[18] = s2; v[19] = t1;
    v[20] = x2; v[21] = y2; v[22] = s2; v[23] = t2;
}


#endif
