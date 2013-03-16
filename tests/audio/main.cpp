#include <stdlib.h>
#include <Cortex2D>

class MainWindow : public CtWindow
{
public:
    MainWindow();
    ~MainWindow();

protected:
    bool init();
    bool event(CtEvent *event);

private:
    CtSoundSample m_sample1;
    CtSoundSample m_sample2;
};


MainWindow::MainWindow()
    : CtWindow("Audio", 320, 240)
{
    printf("Press 't' to toggle mixer activation and '1' and '2' to toggle samples.\n");
}

MainWindow::~MainWindow()
{
    CtSoundMixer::instance()->close();
}

bool MainWindow::init()
{
    CtSoundMixer *mixer = CtSoundMixer::instance();
    mixer->setSpecification(16000, CtSoundMixer::S16LE, 1, 4096);

    if (!mixer->open())
        fprintf(stderr, "Error: Unable to open sound mixer\n");

    if (!m_sample1.loadWav("./wind.wav"))
        fprintf(stderr, "Error: Unable to open 'wind.wav' file\n");

    if (!m_sample2.loadWav("./hit.wav"))
        fprintf(stderr, "Error: Unable to open 'wind.wav' file\n");

    m_sample1.setLoopCount(-1);
    m_sample2.setLoopCount(3);

    mixer->addSample(&m_sample1);
    mixer->addSample(&m_sample2);

    m_sample1.play();
    m_sample2.play();

    return true;
}

bool MainWindow::event(CtEvent *event)
{
    if (event->type() == CtEvent::KeyPress) {
        CtKeyEvent *ev = static_cast<CtKeyEvent *>(event);

        switch (ev->key()) {
        case 't': {
            CtSoundMixer *mixer = CtSoundMixer::instance();
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

    return CtWindow::event(event);
}


int main(int argc, char *argv[])
{
    CtBasicApplication<MainWindow> app(argc, argv);
    return ctMain(argc, argv, &app);
}
