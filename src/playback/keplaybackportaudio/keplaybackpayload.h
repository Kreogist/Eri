#ifndef KEPLAYBACKPAYLOAD_H
#define KEPLAYBACKPAYLOAD_H

#include "keportaudioglobal.h"

#include <QObject>

class KEPlaybackPayload : public QObject
{
    Q_OBJECT
public:
    static void playDecoderData();
    explicit KEPlaybackPayload(QObject *parent = 0);
    ~KEPlaybackPayload();

signals:

public slots:

private:
    static KEPortAudioStream *m_streamData;
};

#endif // KEPLAYBACKPAYLOAD_H
