#include <stdlib.h>
#include <FlApplication>
#include <FlSceneView>
#include <FlGL>
#include <FlSceneItem>
#include <FlSceneRect>
#include <FlSceneImage>

class MainWindow : public FlSceneView
{
public:
    MainWindow();

protected:
    bool init();

private:
    FlTexture m_texture;
};

class PulsatingRect : public FlSceneRect
{
public:
    PulsatingRect(FlSceneItem *parent = 0)
        : FlSceneRect(parent), m_pressed(false) {}

protected:
    void advance(fluint phase);
    void mousePressEvent(FlMouseEvent *) { m_pressed = true; }
    void mouseReleaseEvent(FlMouseEvent *) { m_pressed = false; }

private:
    bool m_pressed;
};

void PulsatingRect::advance(fluint ms)
{
    flreal step = 0.02;
    flreal value = (r() + step > 1.0f) ? 0.0f : (r() + step);

    if (m_pressed)
        setColor(value, 0, 0);
    else
        setColor(value, value, 0);
}


MainWindow::MainWindow()
    : FlSceneView("Items", 500, 400)
{

}

bool MainWindow::init()
{
    FlApplication *app = FlApplication::instance();
    m_texture.loadTGA(app->applicationDir() + "/flag.tga");

    // create root item
    FlSceneImage *root = new FlSceneImage(&m_texture, 0);
    root->setX(50);
    root->setY(90);
    root->setRotation(10);
    root->setTransformOrigin(root->width() / 2,
                             root->height() / 2);

    // child of root
    FlSceneImage *item1 = new FlSceneImage(&m_texture, root);
    item1->setX(20);
    item1->setY(20);
    item1->setZ(2); // should be in front of rect1
    item1->setRotation(-20);
    item1->scale(0.5, 0.3);
    item1->setOpacity(0.7);

    // child of item1
    FlSceneImage *item2 = new FlSceneImage(&m_texture, item1);
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
    FlApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
