#include <Cortex2D>
#include "player.h"
#include "button.h"

class MainWindow : public CtSceneView
{
public:
    MainWindow();

protected:
    bool init();
    void onWalkClicked();
    void onFlyClicked();

private:
    Player *m_player;
};

MainWindow::MainWindow()
    : CtSceneView("Sprite", 500, 500),
      m_player(0)
{

}

bool MainWindow::init()
{
    CtApplication *app = CtApplication::instance();

    // load default texture
    CtAtlasTexture *texture = new CtAtlasTexture();
    texture->loadAtlas(app->applicationDir() + "/images/default.atlas");

    // insert default texture into cache
    CtTextureCache::insert("default", texture);

    // create root item
    CtSceneItem *root = new CtSceneItem();

    // create player sprite
    m_player = new Player(root);
    m_player->setX(100);
    m_player->setY(100);

    // create walk button
    Button *walkButton = new Button("default", root);
    walkButton->setNormalFrame("buttons/btn_walking.png");
    walkButton->setPressedFrame("buttons/btn_walking_pressed.png");
    walkButton->setX(100);
    walkButton->setY(300);
    walkButton->clicked.connect(this, &MainWindow::onWalkClicked);

    // create fly button
    Button *flyButton = new Button("default", root);
    flyButton->setNormalFrame("buttons/btn_runfly.png");
    flyButton->setPressedFrame("buttons/btn_runfly_pressed.png");
    flyButton->setX(300);
    flyButton->setY(300);
    flyButton->clicked.connect(this, &MainWindow::onFlyClicked);

    // add root to the scene
    setRootItem(root);

    return true;
}

void MainWindow::onWalkClicked()
{
    m_player->setState(Player::WalkingState);
}

void MainWindow::onFlyClicked()
{
    m_player->setState(Player::FlyingState);
}

int main(int argc, char *argv[])
{
    CtBasicApplication<MainWindow> app(argc, argv);
    return ctMain(argc, argv, &app);
}
