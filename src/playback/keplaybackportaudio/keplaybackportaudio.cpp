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

#include "keplaybackportaudio.h"

#include "knconnectionhandler.h"

#include <QDebug>

KEPlaybackPortAudio::KEPlaybackPortAudio(QObject *parent) :
    KEPlaybackBase(parent)
{
    //Initial the PortAudio, get PortAudio global.
    m_portAudioGlobal=KEPortAudioGlobal::instance();
    //Link resample update.
    connect(m_portAudioGlobal, &KEPortAudioGlobal::requireUpdateResample,
            this, &KEPlaybackPortAudio::onActionUpdateResample);
}

KEPlaybackPortAudio::~KEPlaybackPortAudio()
{
    blockSignals(true);
    //Force close the stream.
    Pa_CloseStream(m_stream);
}

void KEPlaybackPortAudio::reset()
{
    //Block all the signals.
    blockSignals(true);
    //Reset the state.
    m_state=StoppedState;
    //Free the stream.
    if(m_stream!=NULL)
    {
        //Stop the stream if it's still activated.
        if(!Pa_IsStreamStopped(m_stream))
        {
            Pa_StopStream(m_stream);
        }
        //Close the stream.
        Pa_CloseStream(m_stream);
        //Reset the stream data.
        m_stream=NULL;
    }
    //Release the signal blocks.
    blockSignals(false);
}

bool KEPlaybackPortAudio::setDecoder(KEDecoderBase *decoder)
{
    //Reset the playback.
    reset();
    //Save the decoder.
    m_decoder=decoder;
    return true;
}

void KEPlaybackPortAudio::start()
{
    //Check the state first.
    //If state is PlayingState already or decoder is NULL, exit directly.
    if(m_state==PlayingState || m_decoder==nullptr)
    {
        return;
    }
    //Open the default stream to current stream.
    startDefaultStream();
    //Change the state.
    setPlaybackState(PlayingState);
    //Do the payload.
    decodeAndPlay();
}

void KEPlaybackPortAudio::pause()
{
    //Check the state.
    if(m_state!=PausedState)
    {
        //Close the stream.
        PaError pauseError=Pa_CloseStream(&m_stream);
        if(pauseError!=paNoError)
        {
            //!FIXME: Set error to "Cannot pause stream".
            return;
        }
        //Change the state.
        setPlaybackState(PausedState);
    }
}

void KEPlaybackPortAudio::stop()
{
    if(m_state!=StoppedState)
    {
        //Close the stream.
        PaError stopError=Pa_CloseStream(&m_stream);
        if(stopError!=paNoError)
        {
            //!FIXME: Set error to "Cannot stop stream".
            return;
        }
        //Move decoder back to the postion 0.
        if(m_decoder!=nullptr)
        {
            m_decoder->seek(0);
        }
        //Change the state.
        setPlaybackState(StoppedState);
    }
}

int KEPlaybackPortAudio::state() const
{
    return m_state;
}

void KEPlaybackPortAudio::onActionUpdateResample()
{
    //Check the state is playing state.
    if(m_state==PlayingState)
    {
        //Close the current stream.
        Pa_CloseStream(m_stream);
        //Restart a stream to apply the new resample settings.
        startDefaultStream();
    }
}

void KEPlaybackPortAudio::decodeAndPlay()
{
    //Get the output data.
    KEAudioBufferData outputBuffer=m_decoder->decodeData();
    while(!outputBuffer.data.isEmpty() && m_state==PlayingState)
    {
        //Update the position, timestamp is the position.
        emit positionChanged(outputBuffer.timestamp);
        //Write it to output device, according to the information.
        PaError writeError=Pa_WriteStream(m_stream,
                                          outputBuffer.data.constData(),
                                          outputBuffer.frameCount);
        //Check if there's any error.
        if(writeError!=paNoError)
        {
            qDebug()<<Pa_GetErrorText(writeError);
        }
        //Get the next buffer.
        outputBuffer=m_decoder->decodeData();
        //Ask thread to do other things.
        QApplication::processEvents();
    }
}

inline bool KEPlaybackPortAudio::startDefaultStream()
{
    //Initial the stream according to the decoder.
    PaError streamError=Pa_OpenDefaultStream(&m_stream,
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
    //Save the output latency.
    m_outputLatency=Pa_GetStreamInfo(m_stream)->outputLatency;
    //Start the stream.
    return (paNoError==Pa_StartStream(m_stream));
}

inline void KEPlaybackPortAudio::setPlaybackState(const int &state)
{
    //Save the state.
    m_state=state;
    //Emit state changed signal.
    emit stateChanged(m_state);
}
