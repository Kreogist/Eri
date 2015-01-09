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
#include "keplaybackportaudio.h"

#include "knconnectionhandler.h"

#include <QDebug>

KEPlaybackPortAudio::KEPlaybackPortAudio(QObject *parent) :
    KEPlaybackBase(parent)
{
    //Initial the PortAudio, get PortAudio global.
    m_portAudioGlobal=KEPortAudioGlobal::instance();
    //Link the play loop.
    connect(this, &KEPlaybackPortAudio::playNextPacket,
            this, &KEPlaybackPortAudio::onActionPlayNextPacket);
}

KEPlaybackPortAudio::~KEPlaybackPortAudio()
{
    blockSignals(true);
    //Force close the stream.
    Pa_CloseStream(m_stream);
}

void KEPlaybackPortAudio::reset()
{
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
    //Release the decoder.
    if(m_decoder!=nullptr)
    {
        m_decoder=nullptr;
    }
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
    //Clear the stop flag.
    m_stopFlag=false;
    m_pauseFlag=false;
    //Initial the stream according to the decoder.
    PaError streamError=Pa_OpenDefaultStream(&m_stream,
                                             0,
                                             2,
                                             paInt16,
                                             44100,
                                             0,
                                             NULL,
                                             NULL);
    //Check the error.
    if(streamError!=paNoError)
    {
        return;
    }
    //Save the output latency.
    m_outputLatency=Pa_GetStreamInfo(m_stream)->outputLatency;
    //Start the stream.
    Pa_StartStream(m_stream);
    //Emit play signal.
    emit playNextPacket();
}

void KEPlaybackPortAudio::pause()
{
    //Set pause flag.
    m_pauseFlag=true;
    //Close the stream.
    Pa_CloseStream(&m_stream);
}

void KEPlaybackPortAudio::stop()
{
    //Set stop flag.
    m_stopFlag=true;
    //Close the stream.
    Pa_CloseStream(&m_stream);
    //Move back to the first.
    m_decoder->seek(0);
}

void KEPlaybackPortAudio::onActionPlayNextPacket()
{
    //Get the output data.
    BufferData outputBuffer=m_decoder->decodeData();
    if(m_pauseFlag || m_stopFlag)
    {
        return;
    }
    if(outputBuffer.data.isEmpty())
    {
        stop();
        return;
    }
    //Write it to output device, according to the information.
    PaError writeError=Pa_WriteStream(m_stream,
                                      outputBuffer.data.constData(),
                                      outputBuffer.frameCount);
    //Check if there's any error.
    if(writeError!=paNoError)
    {
        qDebug()<<Pa_GetErrorText(writeError);
    }
    //Ask to play the next packet.
    emit playNextPacket();
}
