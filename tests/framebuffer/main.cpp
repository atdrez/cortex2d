#include <stdlib.h>
#include <Cortex2D>


static const char shaderGlow[] = "                                      \
    uniform sampler2D ct_Texture0;                                      \
    uniform mediump float ct_Opacity;                                   \
    uniform mediump float value;                                        \
    varying mediump vec2 ct_TexCoord0;                                  \
                                                                        \
    void main()                                                         \
    {                                                                   \
        gl_FragColor = texture2D(ct_Texture0, ct_TexCoord0);            \
        gl_FragColor.r *= value;                                        \
        gl_FragColor.a *= ct_Opacity;                                   \
    }                                                                   \
    ";


static const char shaderDesaturate[] = "                                \
    uniform sampler2D ct_Texture0;                                      \
    uniform highp float ct_Opacity;                                     \
    varying highp vec2 ct_TexCoord0;                                    \
    uniform highp float value;                                          \
                                                                        \
    void main() {                                                       \
        lowp vec4 texColor = texture2D(ct_Texture0, ct_TexCoord0.st);   \
        lowp float gray = (texColor.r + texColor.g + texColor.b) / 3.0; \
        gl_FragColor = mix(texColor, vec4(gray, gray, gray, texColor.a), value) * ct_Opacity; \
    }                                                                   \
    ";


class GroupView : public CtSceneFrameBuffer
{
public:
    GroupView(const char *fshader, CtSceneItem *parent);

    void setValue(ctreal v);

private:
    CtShaderEffect *m_effect;
    CtShaderUniform *m_uniform;
};


class MainWindow : public CtSceneView
{
public:
    MainWindow();

protected:
    bool init();

private:
    CtTexture m_texture1;
    CtTexture m_texture2;
};


GroupView::GroupView(const char *fshader, CtSceneItem *parent)
    : CtSceneFrameBuffer(parent)
{
    setBufferSize(400, 400);

    m_effect = new CtShaderEffect(CtShaderEffect::Texture);
    m_effect->setFragmentShader(fshader);

    m_uniform = new CtShaderUniform("value", CtShaderUniform::FloatType);
    m_uniform->setValue((ctreal)1.0);
    m_effect->addUniform(m_uniform);

    setShaderEffect(m_effect);
}

void GroupView::setValue(ctreal v)
{
    m_uniform->setValue(v);
}


MainWindow::MainWindow()
    : CtSceneView("FrameBuffer", 600, 400)
{

}

bool MainWindow::init()
{
    CtApplication *app = CtApplication::instance();

    m_texture1.load(app->applicationDir() + "/trash.tga");
    m_texture2.load(app->applicationDir() + "/umbrella.tga");

    CtSceneItem *root = new CtSceneItem();

    CtSceneImage *item0 = new CtSceneImage(&m_texture1, root);
    item0->setX(20);
    item0->setY(20);

    CtSceneImage *item1 = new CtSceneImage(&m_texture2, item0);
    item1->setX(40);
    item1->setY(50);

    // group 1 (desaturated)
    GroupView *group1 = new GroupView(shaderDesaturate, root);
    group1->setX(220);
    group1->setY(20);
    group1->setWidth(400);
    group1->setHeight(400);
    group1->setValue(0.95);

    CtSceneImage *item2 = new CtSceneImage(&m_texture1, group1);

    CtSceneImage *item3 = new CtSceneImage(&m_texture2, item2);
    item3->setX(40);
    item3->setY(50);

    // group 2 (red channel)
    GroupView *group2 = new GroupView(shaderGlow, root);
    group2->setX(20);
    group2->setY(200);
    group2->setWidth(400);
    group2->setHeight(400);
    group2->setValue(0.1);

    CtSceneImage *item4 = new CtSceneImage(&m_texture1, group2);

    CtSceneImage *item5 = new CtSceneImage(&m_texture2, item4);
    item5->setX(40);
    item5->setY(50);

    // group 3 (desaturated + red channel)
    GroupView *group3 = new GroupView(shaderDesaturate, item4);
    group3->setX(220);
    group3->setWidth(400);
    group3->setHeight(400);
    group3->setValue(0.9);
    group3->setRotation(40);

    CtSceneImage *item6 = new CtSceneImage(&m_texture1, group3);

    CtSceneImage *item7 = new CtSceneImage(&m_texture2, group3);
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
