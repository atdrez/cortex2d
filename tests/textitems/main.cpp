#include <stdlib.h>
#include <Cortex2D>

class MainWindow : public CtSceneView
{
public:
    MainWindow();

protected:
    bool init();

    CtTextureFont *m_font1;
    CtTextureFont *m_font2;
    CtTextureFont *m_font3;
};

MainWindow::MainWindow()
    : CtSceneView("Text Items", 1000, 400)
{

}

bool MainWindow::init()
{
    CtApplication *app = CtApplication::instance();

    CtFontManager::registerFont("futurex-40", "./futurex.ttf", 40);
    CtFontManager::registerFont("pusab-20", "./pusab.ttf", 20);
    CtFontManager::registerFont("toothbrush-30", "./toothbrush.otf", 30);

    m_font1 = CtFontManager::findFont("futurex-40");
    m_font2 = CtFontManager::findFont("toothbrush-30");
    m_font3 = CtFontManager::findFont("pusab-20");

    CT_ASSERT(!m_font1 || !m_font2, "Unable to load fonts");

    // create root item
    CtSceneItem *root = new CtSceneItem();

    CtSceneText *item1 = new CtSceneText(root);
    item1->setX(20);
    item1->setY(40);
    item1->setFont(m_font1);
    item1->setColor(CtColor(0.5, 0.8, 1));
    item1->setText("The Quick Brown Fox Jumps Over The Lazy Dog! 0123456789");

    CtSceneText *item2 = new CtSceneText(root);
    item2->setX(20);
    item2->setY(100);
    item2->setFont(m_font2);
    item2->setText("The Quick Brown Fox Jumps Over The Lazy Dog! 0123456789");

    CtSceneText *item3 = new CtSceneText(root);
    item3->setX(20);
    item3->setY(160);
    item3->setFont(m_font3);
    item3->setText("The Quick Brown Fox Jumps Over The Lazy Dog! 0123456789");

    CtSceneText *item4 = new CtSceneText(root);
    item4->setX(20);
    item4->setY(220);
    item4->setFont(m_font3);
    item4->setColor(CtColor(1, 0, 0));
    item4->setText("The Quick Brown Fox Jumps Over The Lazy Dog! 0123456789");

    CtSceneText *item5 = new CtSceneText(root);
    item5->setX(20);
    item5->setY(280);
    item5->setRotation(5);
    item5->setFont(m_font1);
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
