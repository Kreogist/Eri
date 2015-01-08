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
}

KEPlaybackPortAudio::~KEPlaybackPortAudio()
{
    ;
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
        return false;
    }
    //Save the output latency.
    m_outputLatency=Pa_GetStreamInfo(m_stream)->outputLatency;
    //Start the stream.
    PaError openStreamError=Pa_StartStream(m_stream);

    //Do a test here.
    BufferData test=m_decoder->decodeData();
    while(!test.data.isEmpty())
    {
        PaError writeError=Pa_WriteStream(m_stream,
                                          test.data.constData(),
                                          test.frameCount);
        if(writeError!=paNoError)
        {
            qDebug()<<Pa_GetErrorText(writeError);
        }
        test=m_decoder->decodeData();
    }
}
