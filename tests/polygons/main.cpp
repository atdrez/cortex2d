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


MainWindow::MainWindow()
    : CtSceneView("Polygons", 800, 400)
{

}

bool MainWindow::init()
{
    CtApplication *app = CtApplication::instance();
    m_texture.load(app->applicationDir() + "/bg.png");

    // create root item
    CtSprite *root = new CtSprite();

    CtVector<CtPoint> vertices;
    vertices.append(CtPoint(50.0f, 10.0f));
    vertices.append(CtPoint(10.0f, 160.0f));
    vertices.append(CtPoint(180.0f, 190.0f));
    vertices.append(CtPoint(150.0f, 60.0f));

    CtImagePolygonSprite *poly1 = new CtImagePolygonSprite(&m_texture, root);
    poly1->setWidth(180);
    poly1->setHeight(190);
    poly1->setVertices(vertices);

    CtImagePolygonSprite *poly2 = new CtImagePolygonSprite(&m_texture, root);
    poly2->setX(200);
    poly2->setWidth(180);
    poly2->setHeight(190);
    poly2->setVertices(vertices);
    poly2->setFillMode(CtImagePolygonSprite::TileVertically);

    CtImagePolygonSprite *poly3 = new CtImagePolygonSprite(&m_texture, root);
    poly3->setX(400);
    poly3->setWidth(180);
    poly3->setHeight(190);
    poly3->setVertices(vertices);
    poly3->setFillMode(CtImagePolygonSprite::TileHorizontally);

    CtImagePolygonSprite *poly4 = new CtImagePolygonSprite(&m_texture, root);
    poly4->setX(600);
    poly4->setWidth(180);
    poly4->setHeight(190);
    poly4->setVertices(vertices);
    poly4->setFillMode(CtImagePolygonSprite::Tile);

    // add root to scene
    setRootItem(root);

    return true;
}

int main(int argc, char *argv[])
{
    CtBasicApplication<MainWindow> app(argc, argv);
    return ctMain(argc, argv, &app);
}
