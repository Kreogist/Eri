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
#include <QApplication>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrent>

#include "keplaybackportaudio.h"
#include "keportaudiopayload.h"

#include "knconnectionhandler.h"

#include <QDebug>

KEPlaybackPortAudio::KEPlaybackPortAudio(QObject *parent) :
    KEPlaybackBase(parent)
{
    //Initial the PortAudio, get PortAudio global.
    m_portAudioGlobal=KEPortAudioGlobal::instance();
    //Initial the stream data.
    m_streamData.state=StoppedState;
}

KEPlaybackPortAudio::~KEPlaybackPortAudio()
{
    blockSignals(true);
    //Stop the stream.
    stop();
}

void KEPlaybackPortAudio::reset()
{
    //Block all the signals.
    blockSignals(true);
    //Stop the thread.
    stop();
    //Free the stream.
    if(m_streamData.stream!=nullptr)
    {
        //Close the stream.
        Pa_AbortStream(m_streamData.stream);
        //Reset the stream data.
        m_streamData.stream=nullptr;
    }
    //Release the signal blocks.
    blockSignals(false);
    //Update the position.
    positionChanged(0);
}

bool KEPlaybackPortAudio::setDecoder(KEDecoderBase *decoder)
{
    //Reset the playback.
    reset();
    //Save the decoder.
    m_streamData.decoder=decoder;
    return true;
}

void KEPlaybackPortAudio::start()
{
    //Check the state first.
    //If state is PlayingState already or decoder is NULL, exit directly.
    if(m_streamData.state==PlayingState || m_streamData.decoder==nullptr)
    {
        return;
    }
    if(m_streamData.stream==nullptr)
    {
        //Open the default stream to current stream.
        openDefaultStream();
    }
    //Start the stream.
    Pa_StartStream(m_streamData.stream);
    //Change the state.
    setPlaybackState(PlayingState);
    //Do the payload.
    m_playingPayload=QtConcurrent::run(KEPortAudioPayload::decodeAndPlay, &m_streamData, this);
}

void KEPlaybackPortAudio::pause()
{
    //Check the state.
    if(m_streamData.state!=PausedState && m_streamData.state!=StoppedState)
    {
        //Change the state.
        m_streamData.state=PausedState;
        //Wating for pay load finished playing.
        m_playingPayload.waitForFinished();
        //Close the stream.
        PaError pauseError=Pa_AbortStream(m_streamData.stream);
        if(pauseError!=paNoError)
        {
            //!FIXME: Set error to "Cannot pause stream".
            return;
        }
        //Emit changed signal.
        emit stateChanged(m_streamData.state);
    }
}

void KEPlaybackPortAudio::stop()
{
    if(m_streamData.state!=StoppedState)
    {
        //Change the state.
        m_streamData.state=StoppedState;
        //Wating for pay load finished playing.
        m_playingPayload.waitForFinished();
        //If the stream is active, then abort it.
        if(Pa_IsStreamActive(m_streamData.stream)==1)
        {
            //Close the stream.
            PaError stopError=Pa_AbortStream(m_streamData.stream);
            if(paNoError!=stopError)
            {
                //Output error.
                qDebug()<<Pa_GetErrorText(stopError);
            }
        }
        //Reset the stream data.
        m_streamData.stream=nullptr;
        //Seek back to 0.
        if(m_streamData.decoder!=nullptr)
        {
            m_streamData.decoder->seek(0);
        }
        //Emit changed signal.
        emit stateChanged(m_streamData.state);
        //Emit position changed signal.
        emit positionChanged(0);
    }
}

int KEPlaybackPortAudio::state() const
{
    return m_streamData.state;
}

inline bool KEPlaybackPortAudio::openDefaultStream()
{
    //Initial the stream according to the decoder.
    PaError streamError=Pa_OpenDefaultStream(&m_streamData.stream,
                                             0,
                                             m_portAudioGlobal->outputChannels(),
                                             m_portAudioGlobal->getSampleFormat(m_streamData.decoder->sampleFormat()),
                                             m_streamData.decoder->sampleRate(),
                                             0,
                                             NULL,
                                             NULL);
    //Check the error.
    return (paNoError==streamError);
}

inline void KEPlaybackPortAudio::setPlaybackState(const int &state)
{
    //Save the state.
    m_streamData.state=state;
    //Emit state changed signal.
    emit stateChanged(m_streamData.state);
}
