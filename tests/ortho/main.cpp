#include <stdlib.h>
#include <FlApplication>
#include <FlSceneView>
#include <FlGL>
#include <FlMatrix>
#include <FlShaderProgram>

class MainWindow : public FlWindow
{
public:
    MainWindow();

protected:
    bool init();
    void paint();

private:
    FlShaderProgram m_program;
    GLuint m_locPosition;
    GLuint m_locProjection;
    GLuint m_locTexCoord;
    GLuint m_locTexture;
    GLuint m_textureId;
    FlMatrix m_projection;
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
    : FlWindow("Ortho", 320, 240)
{

}

bool MainWindow::init()
{
    // set projection matrix
    m_projection = FlMatrix();
    m_projection.ortho(0, width(), height(), 0, 1, -1);

    // load program
    m_program.loadVertexShader(vShaderStr);
    m_program.loadFragmentShader(fShaderStr);
    m_program.link();

    m_locProjection = FlGL::glGetUniformLocation(m_program.id(), "u_projection");
    m_locPosition = FlGL::glGetAttribLocation(m_program.id(), "a_position");
    m_locTexCoord = FlGL::glGetAttribLocation(m_program.id(), "a_texCoord");
    m_locTexture = FlGL::glGetUniformLocation(m_program.id(), "u_texture");

    // create and bind texture
    FlGL::glGenTextures(1, &m_textureId);

    FlApplication *app = FlApplication::instance();
    FlString path = app->applicationDir() + "/image.tga";

    m_textureId = FlGL::loadTGATexture(path.c_str());
    FlGL::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    FlGL::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

void MainWindow::paint()
{
    // set viewport
    FlGL::glViewport(0, 0, width(), height());

    // clear color buffer
    FlGL::glClearColor(1.0, 1.0, 1.0, 1.0);
    FlGL::glClear(GL_COLOR_BUFFER_BIT);

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
    FlGL::glUniformMatrix4fv(m_locProjection, 1, GL_FALSE, (GLfloat *)m_projection.data());
    FlGL::glVertexAttribPointer(m_locPosition, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    FlGL::glVertexAttribPointer(m_locTexCoord, 2, GL_FLOAT, GL_FALSE, 0, texCoords);

    // enable vertex arrays
    FlGL::glEnableVertexAttribArray(m_locTexCoord);
    FlGL::glEnableVertexAttribArray(m_locPosition);

    // bind texture
    FlGL::glActiveTexture(GL_TEXTURE0);
    FlGL::glBindTexture(GL_TEXTURE_2D, m_textureId);

    // use texture unit 0 (GL_TEXTURE0)
    FlGL::glUniform1i(m_locTexture, 0);

    FlGL::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_program.release();

    swapBuffers();
}

int main(int argc, char *argv[])
{
    FlApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
