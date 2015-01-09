#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QThread;
class KEDecoderBase;
class KEPlaybackBase;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:

public slots:
    void open();
    void play();
    void pause();
    void stop();

private:
    KEDecoderBase *m_decoder;
    KEPlaybackBase *m_playback;
    QThread *m_decoderThread, *m_playbackThread;
};

#endif // MAINWINDOW_H
