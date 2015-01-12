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
    //Link resample update.
    connect(m_portAudioGlobal, &KEPortAudioGlobal::requireUpdateResample,
            this, &KEPlaybackPortAudio::onActionUpdateResample);
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
    //Reset the state.
    m_streamData.state=StoppedState;
    //Free the stream.
    if(m_streamData.stream!=NULL)
    {
        //Stop the stream if it's still activated.
        if(!Pa_IsStreamStopped(m_streamData.stream))
        {
            Pa_StopStream(m_streamData.stream);
        }
        //Close the stream.
        Pa_CloseStream(m_streamData.stream);
        //Reset the stream data.
        m_streamData.stream=NULL;
    }
    //Release the signal blocks.
    blockSignals(false);
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
    //Open the default stream to current stream.
    startDefaultStream();
    //Change the state.
    setPlaybackState(PlayingState);
    //Do the payload.
    m_playingPayload=QtConcurrent::run(KEPortAudioPayload::decodeAndPlay, &m_streamData, this);
}

void KEPlaybackPortAudio::pause()
{
    //Check the state.
    if(m_streamData.state!=PausedState)
    {
        //Change the state.
        m_streamData.state=PausedState;
        //Wating for pay load finished playing.
        m_playingPayload.waitForFinished();
        //Close the stream.
        PaError pauseError=Pa_CloseStream(m_streamData.stream);
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
        //Close the stream.
        PaError stopError=Pa_CloseStream(m_streamData.stream);
        if(paNoError!=stopError)
        {
            //Output error.
            qDebug()<<Pa_GetErrorText(stopError);
            return;
        }
        //Seek back to 0.
        if(m_streamData.decoder!=nullptr)
        {
            m_streamData.decoder->seek(0);
        }
        //Emit changed signal.
        emit stateChanged(m_streamData.state);
    }
}

int KEPlaybackPortAudio::state() const
{
    return m_streamData.state;
}

void KEPlaybackPortAudio::onActionUpdateResample()
{
    //Check the state is playing state.
    if(m_streamData.state==PlayingState)
    {
        //Close the current stream.
        Pa_CloseStream(m_streamData.stream);
        //Restart a stream to apply the new resample settings.
        startDefaultStream();
    }
}

inline bool KEPlaybackPortAudio::startDefaultStream()
{
    //Initial the stream according to the decoder.
    PaError streamError=Pa_OpenDefaultStream(&m_streamData.stream,
                                             0,
                                             m_portAudioGlobal->outputChannels(),
                                             m_portAudioGlobal->sampleFormat(),
                                             m_portAudioGlobal->sampleRate(),
                                             0,
                                             NULL,
                                             NULL);
    //Check the error.
    if(streamError!=paNoError)
    {
        return false;
    }
    //Start the stream.
    return (paNoError==Pa_StartStream(m_streamData.stream));
}

inline void KEPlaybackPortAudio::setPlaybackState(const int &state)
{
    //Save the state.
    m_streamData.state=state;
    //Emit state changed signal.
    emit stateChanged(m_streamData.state);
}
