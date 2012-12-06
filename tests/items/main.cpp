#include <stdlib.h>
#include <Cortex2D>

class MainWindow : public CtSceneView
{
public:
    MainWindow();

protected:
    bool init();

private:
    CtTexture m_texture;
};

class PulsatingRect : public CtSceneRect
{
public:
    PulsatingRect(CtSceneItem *parent = 0)
        : CtSceneRect(parent), m_pressed(false) {}

protected:
    void advance(ctuint phase);
    void mousePressEvent(CtMouseEvent *) { m_pressed = true; }
    void mouseReleaseEvent(CtMouseEvent *) { m_pressed = false; }

private:
    bool m_pressed;
};

void PulsatingRect::advance(ctuint ms)
{
    ctreal step = 0.02;
    ctreal value = (r() + step > 1.0f) ? 0.0f : (r() + step);

    if (m_pressed)
        setColor(value, 0, 0);
    else
        setColor(value, value, 0);
}


MainWindow::MainWindow()
    : CtSceneView("Items", 500, 400)
{

}

bool MainWindow::init()
{
    CtApplication *app = CtApplication::instance();
    m_texture.load(app->applicationDir() + "/flag.tga");

    // create root item
    CtSceneImage *root = new CtSceneImage(&m_texture, 0);
    root->setX(50);
    root->setY(90);
    root->setRotation(10);
    root->setTransformOrigin(root->width() / 2,
                             root->height() / 2);

    // child of root
    CtSceneImage *item1 = new CtSceneImage(&m_texture, root);
    item1->setX(20);
    item1->setY(20);
    item1->setZ(2); // should be in front of rect1
    item1->setRotation(-20);
    item1->scale(0.5, 0.3);
    item1->setOpacity(0.7);

    // child of item1
    CtSceneImage *item2 = new CtSceneImage(&m_texture, item1);
    item2->setX(14);
    item2->setY(14);
    item2->scale(0.8, 0.8);
    item2->setRotation(-20);

    // add rect child of item1
    PulsatingRect *rect1 = new PulsatingRect(root);
    rect1->setX(100);
    rect1->setY(60);
    rect1->setWidth(80);
    rect1->setHeight(80);

    // add root to scene
    setRootItem(root);

    return true;
}

int main(int argc, char *argv[])
{
    CtBasicApplication<MainWindow> app(argc, argv);
    return ctMain(argc, argv, &app);
}
