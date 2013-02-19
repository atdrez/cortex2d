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
    CtSceneItem *root = new CtSceneItem();

    CtVector<CtPointReal> vertices;
    vertices.push_back(CtPointReal(50.0f, 10.0f));
    vertices.push_back(CtPointReal(10.0f, 160.0f));
    vertices.push_back(CtPointReal(180.0f, 190.0f));
    vertices.push_back(CtPointReal(150.0f, 60.0f));

    CtSceneImagePoly *poly1 = new CtSceneImagePoly(&m_texture, root);
    poly1->setWidth(180);
    poly1->setHeight(190);
    poly1->setVertices(vertices);

    CtSceneImagePoly *poly2 = new CtSceneImagePoly(&m_texture, root);
    poly2->setX(200);
    poly2->setWidth(180);
    poly2->setHeight(190);
    poly2->setVertices(vertices);
    poly2->setFillMode(CtSceneImage::TileVertically);

    CtSceneImagePoly *poly3 = new CtSceneImagePoly(&m_texture, root);
    poly3->setX(400);
    poly3->setWidth(180);
    poly3->setHeight(190);
    poly3->setVertices(vertices);
    poly3->setFillMode(CtSceneImage::TileHorizontally);

    CtSceneImagePoly *poly4 = new CtSceneImagePoly(&m_texture, root);
    poly4->setX(600);
    poly4->setWidth(180);
    poly4->setHeight(190);
    poly4->setVertices(vertices);
    poly4->setFillMode(CtSceneImage::Tile);

    // add root to scene
    setRootItem(root);

    return true;
}

int main(int argc, char *argv[])
{
    CtBasicApplication<MainWindow> app(argc, argv);
    return ctMain(argc, argv, &app);
}
