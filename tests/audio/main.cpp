#include <stdlib.h>
#include <FlApplication>
#include <FlWindow>
#include <FlSoundMixer>
#include <FlSoundSample>

class MainWindow : public FlWindow
{
public:
    MainWindow();
    ~MainWindow();

protected:
    bool init();
    bool event(FlEvent *event);

private:
    FlSoundSample m_sample1;
    FlSoundSample m_sample2;
};


MainWindow::MainWindow()
    : FlWindow("Audio", 320, 240)
{
    printf("Press 't' to toggle mixer activation and '1' and '2' to toggle samples.\n");
}

MainWindow::~MainWindow()
{
    FlSoundMixer::instance()->close();
}

bool MainWindow::init()
{
    FlSoundMixer *mixer = FlSoundMixer::instance();
    FlString dir = FlApplication::instance()->applicationDir();

    if (!mixer->open())
        fprintf(stderr, "Error: Unable to open sound mixer\n");

    if (!m_sample1.loadWav(dir + "/wind.wav"))
        fprintf(stderr, "Error: Unable to open 'wind.wav' file\n");

    if (!m_sample2.loadWav(dir + "/hit.wav"))
        fprintf(stderr, "Error: Unable to open 'wind.wav' file\n");

    m_sample1.setLoopCount(-1);
    m_sample2.setLoopCount(3);

    mixer->addSample(&m_sample1);
    mixer->addSample(&m_sample2);

    m_sample1.play();
    m_sample2.play();

    return true;
}

bool MainWindow::event(FlEvent *event)
{
    if (event->type() == FlEvent::KeyPress) {
        FlKeyEvent *ev = static_cast<FlKeyEvent *>(event);

        switch (ev->key()) {
        case 't': {
            FlSoundMixer *mixer = FlSoundMixer::instance();
            if (mixer->isActive()) {
                mixer->setActive(false);
                printf("Disabling sound mixer\n");
            } else {
                mixer->setActive(true);
                printf("Enabling sound mixer\n");
            }
            break;
        }
        case '1': {
            if (m_sample1.isRunning()) {
                m_sample1.pause();
                printf("Pausing Wind sound\n");
            } else {
                m_sample1.play();
                printf("Playing Wind sound\n");
            }
            break;
        }
        case '2': {
            if (m_sample2.isRunning()) {
                m_sample2.pause();
                printf("Pausing Pong sound\n");
            } else {
                m_sample2.play();
                printf("Playing Pong sound\n");
            }
            break;
        }
        default:
            break;
        }
    }

    return FlWindow::event(event);
}


int main(int argc, char *argv[])
{
    FlApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
