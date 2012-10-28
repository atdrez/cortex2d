#include <FlApplication>
#include <FlSceneView>
#include <FlSceneItem>


class MainWindow : public FlSceneView
{
public:
    MainWindow();

protected:
    bool init();

private:
    FlAtlasTexture m_texture;
};


class BlockFragments : public FlSceneFragments
{
public:
    struct Info {
        bool moveTop;
        bool moveLeft;
    };

    BlockFragments(FlAtlasTexture *texture, FlSceneItem *item);
    ~BlockFragments();

    void init();
    void advance(fluint ms);

protected:
    void initFragment(const FlString &name,
                      flreal x, flreal y, flreal w, flreal h);

    FlList<Info *> m_info;
};


BlockFragments::BlockFragments(FlAtlasTexture *texture, FlSceneItem *parent)
    : FlSceneFragments(texture, parent)
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

void BlockFragments::initFragment(const FlString &name,
                                  flreal x, flreal y, flreal w, flreal h)
{
    FlAtlasTexture *atlas = static_cast<FlAtlasTexture *>(texture());

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

void BlockFragments::advance(fluint ms)
{
    setRotation(rotation() + 0.5);

    const flreal w = width();
    const flreal h = height();
    FlList<Fragment *> frags = fragments();

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
    : FlSceneView("Fragments", 400, 400)
{

}

bool MainWindow::init()
{
    FlApplication *app = FlApplication::instance();
    bool ok = m_texture.loadAtlas(app->applicationDir() + "/blocks.atlas");

    if (!ok) {
        FL_WARNING("Unable to load atlas");
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
    FlApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
