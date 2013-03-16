#include <stdlib.h>
#include <Cortex2D>

static const char vertexShader[] = "         \
    uniform mediump mat4 ct_Matrix;          \
    attribute mediump vec2 ct_Vertex;        \
    attribute mediump vec2 ct_TexCoord;      \
    varying mediump vec2 ct_TexCoord0;       \
                                             \
    void main()                              \
    {                                        \
        ct_TexCoord0 = ct_TexCoord;                          \
        gl_Position = ct_Matrix * vec4(ct_Vertex, 1.0, 1.0); \
    }                                                        \
   ";

static const char colorizeShader[] = "                                  \
    uniform mediump float value;                                        \
    uniform sampler2D ct_Texture0;                                      \
    uniform mediump float ct_Opacity;                                   \
    varying mediump vec2 ct_TexCoord0;                                  \
                                                                        \
    void main()                                                         \
    {                                                                   \
        gl_FragColor = texture2D(ct_Texture0, ct_TexCoord0);            \
        gl_FragColor.r *= value;                                        \
        gl_FragColor.a *= ct_Opacity;                                   \
    }                                                                   \
    ";

static const char desaturateShader[] = "                                \
    uniform mediump float value;                                        \
    uniform sampler2D ct_Texture0;                                      \
    uniform mediump float ct_Opacity;                                   \
    varying mediump vec2 ct_TexCoord0;                                  \
                                                                        \
    void main() {                                                       \
        mediump vec4 color = texture2D(ct_Texture0, ct_TexCoord0.st);   \
        mediump float gray = (color.r + color.g + color.b) / 3.0;       \
        mediump vec4 newColor = vec4(gray, gray, gray, color.a);        \
                                                                        \
        gl_FragColor = mix(color, newColor, value);                     \
        gl_FragColor.a *= ct_Opacity;                                   \
    }                                                                   \
    ";


class GroupView : public CtFrameBufferSprite
{
public:
    GroupView(const CtString &programId, CtSprite *parent);
    ~GroupView();

    void setValue(ctreal v);

private:
    CtShaderEffect *mEffect;
    CtShaderUniform *mUniform;
};


class MainWindow : public CtSceneView
{
public:
    MainWindow();

protected:
    bool init();

private:
    CtTexture mTexture1;
    CtTexture mTexture2;
};


GroupView::GroupView(const CtString &programId, CtSprite *parent)
    : CtFrameBufferSprite(parent)
{
    setBufferSize(400, 400);

    CtShaderProgram *program = CtPool<CtShaderProgram>::value(programId);
    CT_ASSERT(program != 0);

    mEffect = new CtShaderEffect(program);

    mUniform = new CtShaderUniform("value", CtShaderUniform::FloatType);
    mUniform->setValue((ctreal)1.0);

    mEffect->addUniform(mUniform);

    setShaderEffect(mEffect);
}

GroupView::~GroupView()
{
    delete mEffect;
    delete mUniform;
}

void GroupView::setValue(ctreal v)
{
    mUniform->setValue(v);
}


MainWindow::MainWindow()
    : CtSceneView("FrameBuffer", 600, 400)
{

}

bool MainWindow::init()
{
    CtApplication *app = CtApplication::instance();
    mTexture1.load(app->applicationDir() + "/trash.tga");
    mTexture2.load(app->applicationDir() + "/umbrella.tga");

    // create colorize program
    CtShaderProgram *colorizeProgram = new CtShaderProgram();
    colorizeProgram->link(vertexShader, colorizeShader);
    CtPool<CtShaderProgram>::insert("colorize", colorizeProgram);

    // create desaturate program
    CtShaderProgram *desaturateProgram = new CtShaderProgram();
    desaturateProgram->link(vertexShader, desaturateShader);
    CtPool<CtShaderProgram>::insert("desaturate", desaturateProgram);

    CtSprite *root = new CtSprite();

    CtImageSprite *item0 = new CtImageSprite(&mTexture1, root);
    item0->setX(20);
    item0->setY(20);

    CtImageSprite *item1 = new CtImageSprite(&mTexture2, item0);
    item1->setX(40);
    item1->setY(50);

    // group 1 (desaturated)
    GroupView *group1 = new GroupView("desaturate", root);
    group1->setX(220);
    group1->setY(20);
    group1->setWidth(400);
    group1->setHeight(400);
    group1->setValue(0.95);

    CtImageSprite *item2 = new CtImageSprite(&mTexture1, group1);

    CtImageSprite *item3 = new CtImageSprite(&mTexture2, item2);
    item3->setX(40);
    item3->setY(50);

    // group 2 (red channel)
    GroupView *group2 = new GroupView("colorize", root);
    group2->setX(20);
    group2->setY(200);
    group2->setWidth(400);
    group2->setHeight(400);
    group2->setValue(0.1);

    CtImageSprite *item4 = new CtImageSprite(&mTexture1, group2);

    CtImageSprite *item5 = new CtImageSprite(&mTexture2, item4);
    item5->setX(40);
    item5->setY(50);

    // group 3 (desaturated + red channel)
    GroupView *group3 = new GroupView("desaturate", item4);
    group3->setX(220);
    group3->setWidth(400);
    group3->setHeight(400);
    group3->setValue(0.9);
    group3->setRotation(40);

    CtImageSprite *item6 = new CtImageSprite(&mTexture1, group3);

    CtImageSprite *item7 = new CtImageSprite(&mTexture2, group3);
    item7->setX(40);
    item7->setY(50);

    setRootItem(root);

    return true;
}


int main(int argc, char *argv[])
{
    CtBasicApplication<MainWindow> app(argc, argv);
    return ctMain(argc, argv, &app);
}
