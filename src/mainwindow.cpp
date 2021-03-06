#include <QFileDialog>
#include <QBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QThread>
#include <QSlider>

#include "keplayer.h"

#include "decoder/kedecoderffmpeg/kedecoderffmpeg.h"

#ifdef ENABLE_PORTAUDIO
#include "playback/keplaybackportaudio/keplaybackportaudio.h"
#endif
#ifdef ENABLE_OPENAL
#include "playback/keplaybackopenal/keplaybackopenal.h"
#endif

#include "mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QWidget *container=new QWidget(this);
    setCentralWidget(container);
    QBoxLayout *controlCenter=new QBoxLayout(QBoxLayout::TopToBottom, container);
    container->setLayout(controlCenter);
    QBoxLayout *buttons1=new QBoxLayout(QBoxLayout::LeftToRight,
                                        controlCenter->widget());
    controlCenter->addLayout(buttons1);
    QPushButton *buttonOpen1=new QPushButton("Open", this);
    connect(buttonOpen1, SIGNAL(clicked()), this, SLOT(open1()));
    buttons1->addWidget(buttonOpen1);
    QPushButton *buttonPlay1=new QPushButton("Play", this);
    connect(buttonPlay1, SIGNAL(clicked()), this, SLOT(play1()));
    buttons1->addWidget(buttonPlay1);
    QPushButton *buttonPause1=new QPushButton("Pause", this);
    connect(buttonPause1, SIGNAL(clicked()), this, SLOT(pause1()));
    buttons1->addWidget(buttonPause1);
    QPushButton *buttonStop1=new QPushButton("Stop", this);
    connect(buttonStop1, SIGNAL(clicked()), this, SLOT(stop1()));
    buttons1->addWidget(buttonStop1);
    QSlider *progress1=new QSlider(Qt::Horizontal, this);
    controlCenter->addWidget(progress1);

    QBoxLayout *buttons2=new QBoxLayout(QBoxLayout::LeftToRight,
                                        controlCenter->widget());
    controlCenter->addLayout(buttons2);
    QPushButton *buttonOpen2=new QPushButton("Open", this);
    connect(buttonOpen2, SIGNAL(clicked()), this, SLOT(open2()));
    buttons2->addWidget(buttonOpen2);
    QPushButton *buttonPlay2=new QPushButton("Play", this);
    connect(buttonPlay2, SIGNAL(clicked()), this, SLOT(play2()));
    buttons2->addWidget(buttonPlay2);
    QPushButton *buttonPause2=new QPushButton("Pause", this);
    connect(buttonPause2, SIGNAL(clicked()), this, SLOT(pause2()));
    buttons2->addWidget(buttonPause2);
    QPushButton *buttonStop2=new QPushButton("Stop", this);
    connect(buttonStop2, SIGNAL(clicked()), this, SLOT(stop2()));
    buttons2->addWidget(buttonStop2);
    QSlider *progress2=new QSlider(Qt::Horizontal, this);
    controlCenter->addWidget(progress2);

#ifdef ENABLE_OPENAL
    KEPlaybackOpenAL *openAL=new KEPlaybackOpenAL(this);
#endif

    m_player1=new KEPlayer(this);
    m_player2=new KEPlayer(this);
    m_player1->setDecoder(new KEDecoderFfmpeg);
    m_player2->setDecoder(new KEDecoderFfmpeg);
#ifdef ENABLE_PORTAUDIO
    m_player1->setPlayback(new KEPlaybackPortAudio);
    m_player2->setPlayback(new KEPlaybackPortAudio);
#endif
    connect(m_player1, &KEPlayer::durationChanged, [=](const qint64 &duration){progress1->setRange(0, duration);});
    connect(m_player1, &KEPlayer::positionChanged, [=](const qint64 &position){if(m_slide1){progress1->setValue(position);/*qDebug()<<position;*/}});
    connect(progress1, &QSlider::sliderPressed, [=]{m_slide1=false;});
    connect(progress1, &QSlider::sliderReleased, [=]{m_player1->setPosition(progress1->value());m_slide1=true;});

    connect(m_player2, &KEPlayer::durationChanged, [=](const qint64 &duration){progress2->setRange(0, duration);});
    connect(m_player2, &KEPlayer::positionChanged, [=](const qint64 &position){if(m_slide2){progress2->setValue(position);/*qDebug()<<position;*/}});
    connect(progress2, &QSlider::sliderPressed, [=]{m_slide2=false;});
    connect(progress2, &QSlider::sliderReleased, [=]{m_player2->setPosition(progress2->value());m_slide2=true;});
}

MainWindow::~MainWindow()
{
}

void MainWindow::open1()
{
//    KEDecoderFfmpeg *test=new KEDecoderFfmpeg;
//    test->loadLocalFile(QFileDialog::getOpenFileName(this));
//    KEAudioBufferData decodedata=test->decodeData();
////    int counter=1;
////    while(!decodedata.data.isEmpty())
////    {
////        counter++;
////        decodedata=test->decodeData();
////    }
////    qDebug()<<counter;
//    delete test;
    m_player1->loadUrl(QUrl::fromLocalFile(QFileDialog::getOpenFileName(this)));
}

void MainWindow::play1()
{
    m_player1->play();
}

void MainWindow::pause1()
{
    m_player1->pause();
}

void MainWindow::stop1()
{
    m_player1->stop();
}

void MainWindow::open2()
{
    m_player2->loadUrl(QUrl::fromLocalFile(QFileDialog::getOpenFileName(this)));
}

void MainWindow::play2()
{
    m_player2->play();
}

void MainWindow::pause2()
{
    m_player2->pause();
}

void MainWindow::stop2()
{
    m_player2->stop();
}

