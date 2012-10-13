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
    void applyCustomUniforms();

    void drawSolid(const FlMatrix &matrix, flreal width, flreal height,
                   flreal r, flreal g, flreal b, flreal a, flreal opacity);

    void drawTexture(const FlMatrix &projectionMatrix, FlTexture *texture,
                     flreal width, flreal height, flreal opacity,
                     bool tileVertically, bool tileHorizontally, int textureAtlasIndex);

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
