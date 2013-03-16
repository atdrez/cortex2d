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


class ParticleSystem : public CtParticlesSprite
{
public:
    struct Info {
        bool moveTop;
        bool moveLeft;
    };

    ParticleSystem(CtTexture *texture, CtSprite *item);
    ~ParticleSystem();

    void init();
    void advance(ctuint ms);

protected:
    CtList<Info *> m_info;
};


ParticleSystem::ParticleSystem(CtTexture *texture, CtSprite *parent)
    : CtParticlesSprite(texture, parent)
{

}

ParticleSystem::~ParticleSystem()
{
    foreach (Info *info, m_info)
        delete info;
}

void ParticleSystem::init()
{
    for (int i = 0; i < 20; i++) {
        const ctreal x = rand() % 200;
        const ctreal y = rand() % 200;

        Particle *p = new Particle();
        p->setX(x);
        p->setY(y);
        p->setPointSize(rand() % 50 + 10);
        p->setColor(CtColor(1, 0, 0, 1));

        Info *info = new Info();
        info->moveTop = (rand() % 2 == 0);
        info->moveLeft = (rand() % 2 == 0);
        p->setUserData(info);

        addParticle(p);
    }
}

void ParticleSystem::advance(ctuint ms)
{
    const ctreal w = width();
    const ctreal h = height();
    CtVector<Particle *> items = particles();

    foreach (Particle *p, items) {
        Info *info = static_cast<Info *>(p->userData());

        if (p->x() > width())
            info->moveLeft = true;
        else if (p->x() < 0)
            info->moveLeft = false;

        if (p->y() > height())
            info->moveTop = true;
        else if (p->y() < 0)
            info->moveTop = false;

        p->setX(p->x() + (info->moveLeft ? -1 : 1));
        p->setY(p->y() + (info->moveTop ? -1 : 1));
    }
}


MainWindow::MainWindow()
    : CtSceneView("Particles", 400, 400)
{

}

bool MainWindow::init()
{
    CtApplication *app = CtApplication::instance();
    bool ok = m_texture.load(app->applicationDir() + "/particle.png");

    if (!ok) {
        CT_WARNING("Unable to load texture");
        return false;
    }

    // create root item
    ParticleSystem *root = new ParticleSystem(&m_texture, 0);
    root->setX(100);
    root->setY(80);
    root->setWidth(200);
    root->setHeight(200);
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
