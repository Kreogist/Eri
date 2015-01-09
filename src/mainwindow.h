#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class KEPlayer;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:

public slots:
    void open1();
    void play1();
    void pause1();
    void stop1();
    void open2();
    void play2();
    void pause2();
    void stop2();

private:
    KEPlayer *m_player1, *m_player2;
};

#endif // MAINWINDOW_H
