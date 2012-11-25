#include <Cortex2D>


class MainWindow : public CtSceneView
{
public:
    MainWindow();

protected:
    bool init();

private:
    CtAtlasTexture m_texture;
};


class BlockFragments : public CtSceneFragments
{
public:
    struct Info {
        bool moveTop;
        bool moveLeft;
    };

    BlockFragments(CtAtlasTexture *texture, CtSceneItem *item);
    ~BlockFragments();

    void init();
    void advance(ctuint ms);

protected:
    void initFragment(const CtString &name,
                      ctreal x, ctreal y, ctreal w, ctreal h);

    CtList<Info *> m_info;
};


BlockFragments::BlockFragments(CtAtlasTexture *texture, CtSceneItem *parent)
    : CtSceneFragments(texture, parent)
{

}

BlockFragments::~BlockFragments()
{
    foreach (Info *info, m_info)
        delete info;
}

void BlockFragments::init()
{
    for (int i = 0; i < 500; i++) {
        char name[100];
        sprintf(name, "block%d.tga", (i % 4) + 1);
        initFragment(name, rand() % 200, rand() % 200,
                     rand() % 30 + 10, rand() % 30 + 10);
    }
}

void BlockFragments::initFragment(const CtString &name,
                                  ctreal x, ctreal y, ctreal w, ctreal h)
{
    CtAtlasTexture *atlas = static_cast<CtAtlasTexture *>(texture());

    Fragment *frag = new Fragment();
    frag->setX(x);
    frag->setY(y);
    frag->setWidth(w);
    frag->setHeight(h);
    frag->setAtlasIndex(atlas->indexOfKey(name));

    Info *info = new Info();
    info->moveTop = (rand() % 2 == 0);
    info->moveLeft = (rand() % 2 == 0);
    frag->setUserData(info);

    appendFragment(frag);
}

void BlockFragments::advance(ctuint ms)
{
    setRotation(rotation() + 0.5);

    const ctreal w = width();
    const ctreal h = height();
    CtList<Fragment *> frags = fragments();

    foreach (Fragment *frag, frags) {
        Info *info = static_cast<Info *>(frag->userData());

        if (frag->x() + frag->width() > w)
            info->moveLeft = true;
        else if (frag->x() < 0)
            info->moveLeft = false;

        if (frag->y() + frag->height() > h)
            info->moveTop = true;
        else if (frag->y() < 0)
            info->moveTop = false;

        frag->setX(frag->x() + (info->moveLeft ? -1 : 1));
        frag->setY(frag->y() + (info->moveTop ? -1 : 1));
    }
}


MainWindow::MainWindow()
    : CtSceneView("Fragments", 400, 400)
{

}

bool MainWindow::init()
{
    CtApplication *app = CtApplication::instance();
    bool ok = m_texture.loadAtlas(app->applicationDir() + "/blocks.atlas");

    if (!ok) {
        CT_WARNING("Unable to load atlas");
        return false;
    }

    // create root item
    BlockFragments *root = new BlockFragments(&m_texture, 0);
    root->setX(100);
    root->setY(80);
    root->setWidth(200);
    root->setHeight(200);
    root->setRotation(10);
    root->setTransformOrigin(100, 100);
    root->init();

    // add root to scene
    setRootItem(root);

    return true;
}


int main(int argc, char *argv[])
{
    CtBasicApplication<MainWindow> app(argc, argv);
    return ctMain(argc, argv, &app);
}
