#include <stdlib.h>
#include <Cortex2D>

class MainWindow : public CtSceneView
{
public:
    MainWindow();

protected:
    bool init();

    CtFont mFont1;
    CtFont mFont2;
    CtFont mFont3;
    CtFont mFont4;
};

MainWindow::MainWindow()
    : CtSceneView("Text Items", 1000, 400)
{

}

bool MainWindow::init()
{
    CtApplication *app = CtApplication::instance();

    mFont1.loadTTF("./futurex.ttf", 40);
    mFont2.loadTTF("./pusab.ttf", 20);
    mFont3.loadTTF("./toothbrush.otf", 30);
    mFont4.loadBMFont("./bubblegum_sans_regular_22.fnt");

    // create root item
    CtSprite *root = new CtSprite();

    CtTextSprite *item1 = new CtTextSprite(root);
    item1->setX(20);
    item1->setY(40);
    item1->setFont(&mFont1);
    item1->setColor(CtColor(0.5, 0.8, 1));
    item1->setText("The Quick Brown Fox Jumps Over The Lazy Dog! 0123456789");

    CtTextSprite *item2 = new CtTextSprite(root);
    item2->setX(20);
    item2->setY(100);
    item2->setFont(&mFont2);
    item2->setText("The Quick Brown Fox Jumps Over The Lazy Dog! 0123456789");

    CtTextSprite *item3 = new CtTextSprite(root);
    item3->setX(20);
    item3->setY(160);
    item3->setFont(&mFont3);
    item3->setText("The Quick Brown Fox Jumps Over The Lazy Dog! 0123456789");

    CtTextSprite *item4 = new CtTextSprite(root);
    item4->setX(20);
    item4->setY(220);
    item4->setFont(&mFont4);
    item4->setColor(CtColor(1, 0, 0));
    item4->setText("The Quick Brown Fox Jumps Over The Lazy Dog! 0123456789");

    CtTextSprite *item5 = new CtTextSprite(root);
    item5->setX(20);
    item5->setY(280);
    item5->setRotation(5);
    item5->setFont(&mFont1);
    item5->setColor(CtColor(0.2, 0.9, 0.5));
    item5->setText("The Quick Brown Fox Jumps Over The Lazy Dog! 0123456789");

    // add root to scene
    setRootItem(root);

    return true;
}

int main(int argc, char *argv[])
{
    CtBasicApplication<MainWindow> app(argc, argv);
    return ctMain(argc, argv, &app);
}
