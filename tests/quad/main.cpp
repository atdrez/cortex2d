#include <stdlib.h>
#include <Cortex2D>

class MainWindow : public CtWindow
{
public:
    MainWindow();

protected:
    bool init();
    void paint();
    bool event(CtEvent *event);

private:
    CtShaderProgram m_program;
    GLint m_color;
    bool m_highlight;
};


MainWindow::MainWindow()
    : CtWindow("Quad", 320, 240),
      m_highlight(false)
{

}

bool MainWindow::init()
{
    const char vShaderStr[] =
        "attribute vec4 vPosition;    \n"
        "void main()                  \n"
        "{                            \n"
        "   gl_Position = vPosition;  \n"
        "}                            \n";

    const char fShaderStr[] =
        "uniform vec4 uColor;                       \n"
        //"precision mediump float;                   \n"
        "void main()                                \n"
        "{                                          \n"
        "  gl_FragColor = uColor;                   \n"
        "}                                          \n";

    m_program.loadVertexShader(vShaderStr);
    m_program.loadFragmentShader(fShaderStr);
    m_program.link();

    CtGL::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    m_color = CtGL::glGetUniformLocation(m_program.id(), "uColor");

    return true;
}

void MainWindow::paint()
{
    m_program.bind();

    CtGL::glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    CtGL::glClear(GL_COLOR_BUFFER_BIT);

    GLfloat vertices[8];
    GLfloat xMin = (GLfloat)(-0.5);
    GLfloat xMax = (GLfloat)(0.5);
    GLfloat yMin = (GLfloat)(-0.5);
    GLfloat yMax = (GLfloat)(0.5);

    vertices[0] = xMin;
    vertices[1] = yMin;
    vertices[2] = xMax;
    vertices[3] = yMin;
    vertices[4] = xMin;
    vertices[5] = yMax;
    vertices[6] = xMax;
    vertices[7] = yMax;

    CtGL::glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    CtGL::glEnableVertexAttribArray(0);
    CtGL::glUniform4f(m_color, m_highlight ? 1.0 : 0.0, 0.0, 0.0, 1.0);

    CtGL::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    if (CtGL::glGetError() != GL_NO_ERROR)
        CT_WARNING("Failed to render poly");

    m_program.release();

    swapBuffers();
}

bool MainWindow::event(CtEvent *event)
{
    switch (event->type()) {
    case CtEvent::MousePress: {
        m_highlight = true;
        break;
    }
    case CtEvent::MouseMove: {
        CtMouseEvent *ev = static_cast<CtMouseEvent *>(event);
        m_highlight = ev->x() > width() / 2.0;
        break;
    }
    case CtEvent::MouseRelease: {
        m_highlight = false;
        break;
    }
    case CtEvent::KeyPress: {
        CtKeyEvent *ev = static_cast<CtKeyEvent *>(event);
        printf("KeyPress %d : autoRepeat: %d\n",
               ev->key(), ev->isAutoRepeat() ? 1 : 0);
        break;
    }
    case CtEvent::KeyRelease: {
        CtKeyEvent *ev = static_cast<CtKeyEvent *>(event);
        printf("KeyRelease %d : autoRepeat: %d\n",
               ev->key(), ev->isAutoRepeat() ? 1 : 0);
        break;
    }
    default:
        break;
    }

    return CtWindow::event(event);
}


int main(int argc, char *argv[])
{
    CtApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
