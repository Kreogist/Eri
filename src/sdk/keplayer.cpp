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

#include "kedecoderbase.h"
#include "keplaybackbase.h"

#include "keplayer.h"

#include <QDebug>

KEPlayer::KEPlayer(QObject *parent) :
    QObject(parent)
{
    //Initial the decode and playback thread.
    m_playbackThread=new QThread(this);
    m_decoderThread=new QThread(this);
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

void KEPlayer::loadLocalFile(const QString &filePath)
{
    //Reset the playback.
    if(m_playback!=nullptr)
    {
        m_playback->reset();
    }
    if(m_decoder!=nullptr)
    {
        m_decoder->loadLocalFile(filePath);
    }
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
    //Save the playback, and move to playback thread.
    m_playback=playback;
    m_playback->moveToThread(m_playbackThread);
    //Set the decoder to playback.
    m_playback->setDecoder(m_decoder);
}

void KEPlayer::play()
{
    if(m_playback!=nullptr)
    {
        //Start play.
        m_playback->start();
    }
}

void KEPlayer::pause()
{
    if(m_playback!=nullptr)
    {
        //Pause the play back.
        m_playback->pause();
    }
}

void KEPlayer::stop()
{
    if(m_playback!=nullptr)
    {
        //Stop the playback.
        m_playback->stop();
    }
}