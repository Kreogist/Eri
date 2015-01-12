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
#include <QThread>

#include "knconnectionhandler.h"

#include "kedecoderbase.h"
#include "keplaybackbase.h"

#include "keplayer.h"

#include <QDebug>

KEPlayer::KEPlayer(QObject *parent) :
    QObject(parent),
    m_musicState(NoMusic),
    m_decoderThread(new QThread(this)),
    m_playbackThread(new QThread(this)),
    m_playbackHandler(new KNConnectionHandler(this))
{
    //Start the thread right now.
    m_playbackThread->start();
    m_decoderThread->start();
}

KEPlayer::~KEPlayer()
{
    //Quit the threads.
    m_playbackThread->quit();
    m_decoderThread->quit();
    m_playbackThread->wait();
    m_decoderThread->wait();
    //Remove the decoder and playback.
    if(m_playback!=nullptr)
    {
        delete m_playback;
    }
    if(m_decoder!=nullptr)
    {
        delete m_decoder;
    }
}

inline void KEPlayer::loadLocalFile(const QString &filePath)
{
    //Reset the playback.
    if(m_playback!=nullptr)
    {
        m_playback->reset();
    }
    if(m_decoder!=nullptr)
    {
        if(m_decoder->loadLocalFile(filePath))
        {
            emit durationChanged(m_decoder->duration());
        }
    }
}

int KEPlayer::musicState() const
{
    return m_musicState;
}

int KEPlayer::playingState() const
{
    //Return StoppedState if playback is null.
    return m_playback==nullptr?StoppedState:m_playback->state();
}

KEDecoderBase *KEPlayer::decoder() const
{
    return m_decoder;
}

void KEPlayer::setDecoder(KEDecoderBase *decoder)
{
    //Save the decoder, and move to decoder thread.
    m_decoder=decoder;
    m_decoder->moveToThread(m_decoderThread);
    //Check the playback has initial or not, set the decoder.
    if(m_playback!=nullptr)
    {
        m_playback->setDecoder(m_decoder);
    }
}
KEPlaybackBase *KEPlayer::playback() const
{
    return m_playback;
}

void KEPlayer::setPlayback(KEPlaybackBase *playback)
{
    //Check the playback is null or not.
    if(m_playback!=nullptr)
    {
        //Disconnect playbacks signals.
        m_playbackHandler->disconnectAll();
    }
    //Save the playback, and move to playback thread.
    m_playback=playback;
    if(m_playback!=nullptr)
    {
        //Move to working thread.
        m_playback->moveToThread(m_playbackThread);
        //Link the playback.
        m_playbackHandler->addConnectionHandle(
                    connect(m_playback, &KEPlaybackBase::stateChanged,
                            this, &KEPlayer::playingStateChanged));
        m_playbackHandler->addConnectionHandle(
                    connect(m_playback, &KEPlaybackBase::positionChanged,
                            this, &KEPlayer::positionChanged));
        m_playbackHandler->addConnectionHandle(
                    connect(m_playback, &KEPlaybackBase::finished,
                            this, &KEPlayer::finished));
        //Set the decoder to playback.
        m_playback->setDecoder(m_decoder);
    }
}

void KEPlayer::play()
{
    //Check if playback is null, and current state is PlayingState or not.
    if(m_playback!=nullptr && m_playback->state()!=PlayingState)
    {
        //Start play.
        m_playback->start();
    }
}

void KEPlayer::pause()
{
    //Check if playback is null, and current state is PausedState or not.
    if(m_playback!=nullptr && m_playback->state()!=PausedState)
    {
        //Pause the play back.
        m_playback->pause();
    }
}

void KEPlayer::stop()
{
    //Check if playback is null, and current state is StoppedState or not.
    if(m_playback!=nullptr && m_playback->state()!=StoppedState)
    {
        //Stop the playback.
        m_playback->stop();
    }
}

void KEPlayer::loadUrl(const QUrl &url)
{
    //Save the url.
    m_musicUrl=url;
    //Check the url type and play the music.
    if(m_musicUrl.isLocalFile())
    {
        QString localFilePath=m_musicUrl.path();
#ifdef Q_OS_WIN32
        //Remove the first '/'.
        localFilePath.remove(0, 1);
#endif
        loadLocalFile(localFilePath);
    }
}

void KEPlayer::setPosition(qint64 position)
{
    ;
}

void KEPlayer::setVolume(int volume)
{
    ;
}
