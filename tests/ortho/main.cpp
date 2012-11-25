#include <stdlib.h>
#include <Cortex2D>

class MainWindow : public CtWindow
{
public:
    MainWindow();

protected:
    bool init();
    void paint();

private:
    CtShaderProgram m_program;
    GLuint m_locPosition;
    GLuint m_locProjection;
    GLuint m_locTexCoord;
    GLuint m_locTexture;
    GLuint m_textureId;
    CtMatrix m_projection;
};

static const char vShaderStr[] = " \
    uniform mat4 u_projection; \
    attribute vec2 a_position; \
    attribute vec2 a_texCoord; \
    varying vec2 v_texCoord; \
    \
    void main() \
    { \
        v_texCoord = a_texCoord; \
        gl_Position = u_projection * vec4(a_position, 1.0, 1.0); \
        gl_PointSize = 1.0; \
    } \
";

static const char fShaderStr[] = " \
    uniform sampler2D u_texture; \
    varying vec2 v_texCoord; \
    \
    void main() \
    { \
        vec4 abgr = texture2D(u_texture, v_texCoord); \
        gl_FragColor = abgr; \
        gl_FragColor.r = abgr.b; \
        gl_FragColor.b = abgr.r; \
        gl_FragColor.a = 1.0; \
    } \
";


MainWindow::MainWindow()
    : CtWindow("Ortho", 320, 240)
{

}

bool MainWindow::init()
{
    // set projection matrix
    m_projection = CtMatrix();
    m_projection.ortho(0, width(), height(), 0, 1, -1);

    // load program
    m_program.loadVertexShader(vShaderStr);
    m_program.loadFragmentShader(fShaderStr);
    m_program.link();

    m_locProjection = CtGL::glGetUniformLocation(m_program.id(), "u_projection");
    m_locPosition = CtGL::glGetAttribLocation(m_program.id(), "a_position");
    m_locTexCoord = CtGL::glGetAttribLocation(m_program.id(), "a_texCoord");
    m_locTexture = CtGL::glGetUniformLocation(m_program.id(), "u_texture");

    // create and bind texture
    CtGL::glGenTextures(1, &m_textureId);

    CtApplication *app = CtApplication::instance();
    CtString path = app->applicationDir() + "/image.tga";

    m_textureId = CtGL::loadTGATexture(path.c_str());
    CtGL::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    CtGL::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

void MainWindow::paint()
{
    // set viewport
    CtGL::glViewport(0, 0, width(), height());

    // clear color buffer
    CtGL::glClearColor(1.0, 1.0, 1.0, 1.0);
    CtGL::glClear(GL_COLOR_BUFFER_BIT);

    // start program
    m_program.bind();

    GLfloat tw = 128, th = 128; // texture size
    GLfloat sx = 0, sy = 0, sw = 128, sh = 128; // source rect
    GLfloat dx = 80, dy = 40, dw = 128, dh = 128; // dest rect

    const GLfloat vertices[8] = {
        dx, dy, (dx + dw), dy, dx,
        (dy + dh), (dx + dw), (dy + dh)
    };

    const GLfloat texCoords[8] = {
        (sx / tw), (sy / th), ((sx + sw) / tw), (sy / th),
        (sx / tw), ((sy + sh) / th), ((sx + sw) / tw), ((sy + sh) / th)
    };

    // set parameters
    CtGL::glUniformMatrix4fv(m_locProjection, 1, GL_FALSE, (GLfloat *)m_projection.data());
    CtGL::glVertexAttribPointer(m_locPosition, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    CtGL::glVertexAttribPointer(m_locTexCoord, 2, GL_FLOAT, GL_FALSE, 0, texCoords);

    // enable vertex arrays
    CtGL::glEnableVertexAttribArray(m_locTexCoord);
    CtGL::glEnableVertexAttribArray(m_locPosition);

    // bind texture
    CtGL::glActiveTexture(GL_TEXTURE0);
    CtGL::glBindTexture(GL_TEXTURE_2D, m_textureId);

    // use texture unit 0 (GL_TEXTURE0)
    CtGL::glUniform1i(m_locTexture, 0);

    CtGL::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_program.release();
}

int main(int argc, char *argv[])
{
    CtBasicApplication<MainWindow> app(argc, argv);
    return ctMain(argc, argv, &app);
}
