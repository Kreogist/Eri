/*
 * Copyright (C) Kreogist Dev Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KEPLAYER_H
#define KEPLAYER_H

#include <QUrl>

#include "keglobal.h"

#include <QObject>

class KEDecoderBase;
class KEPlaybackBase;
class KNConnectionHandler;
class KEPlayer : public QObject
{
    Q_OBJECT
public:
    explicit KEPlayer(QObject *parent = 0);
    ~KEPlayer();
    KEDecoderBase *decoder() const;
    void setDecoder(KEDecoderBase *decoder);
    KEPlaybackBase *playback() const;
    void setPlayback(KEPlaybackBase *playback);
    int musicState() const;
    int playingState() const;

signals:
    //Music media status.
    void musicStateChanged(int musicState);
    void playingStateChanged(int playingState);

    //Player playing states.
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void volumeChanged(int volume);
    void finished();

public slots:
    void loadUrl(const QUrl &url);

    void play();
    void pause();
    void stop();

    void setPosition(qint64 position);
    void setVolume(int volume);

private:
    //Load different kinds of file.
    inline void loadLocalFile(const QString &filePath);

    //Music player state variables.
    QUrl m_musicUrl;
    int m_musicState;

    //Decoder, playback and their threds.
    KEDecoderBase *m_decoder=nullptr;
    KEPlaybackBase *m_playback=nullptr;
    QThread *m_decoderThread, *m_playbackThread;
    KNConnectionHandler *m_playbackHandler;

};

#endif // KEPLAYER_H
