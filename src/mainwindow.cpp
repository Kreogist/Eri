#include <QFileDialog>
#include <QBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QThread>

#include "decoder/kedecoderffmpeg/kedecoderffmpeg.h"

#include "playback/keplaybackportaudio/keplaybackportaudio.h"


#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QWidget *container=new QWidget(this);
    setCentralWidget(container);
    QBoxLayout *buttons=new QBoxLayout(QBoxLayout::LeftToRight, container);
    container->setLayout(buttons);

    QPushButton *buttonOpen=new QPushButton("Open", this);
    connect(buttonOpen, SIGNAL(clicked()), this, SLOT(open()));
    buttons->addWidget(buttonOpen);
    QPushButton *buttonPlay=new QPushButton("Play", this);
    connect(buttonPlay, SIGNAL(clicked()), this, SLOT(play()));
    buttons->addWidget(buttonPlay);
    QPushButton *buttonPause=new QPushButton("Pause", this);
    connect(buttonPause, SIGNAL(clicked()), this, SLOT(pause()));
    buttons->addWidget(buttonPause);
    QPushButton *buttonStop=new QPushButton("Stop", this);
    connect(buttonStop, SIGNAL(clicked()), this, SLOT(stop()));
    buttons->addWidget(buttonStop);

    m_playbackThread=new QThread(this);
    m_decoderThread=new QThread(this);
    m_decoder=new KEDecoderFfmpeg;
    m_decoder->moveToThread(m_decoderThread);
    m_playback=new KEPlaybackPortAudio;
    m_playback->moveToThread(m_playbackThread);
    m_playback->setDecoder(m_decoder);
    m_playbackThread->start();
    m_decoderThread->start();
}

MainWindow::~MainWindow()
{
    m_playbackThread->quit();
    m_playbackThread->wait();
    m_decoderThread->quit();
    m_decoderThread->wait();
    delete m_playback;
    delete m_decoder;
}

void MainWindow::open()
{
    m_decoder->loadLocalFile(QFileDialog::getOpenFileName(this));
}

void MainWindow::play()
{
    m_playback->start();
}

void MainWindow::pause()
{
    m_playback->pause();
}

void MainWindow::stop()
{
    m_playback->stop();
}

