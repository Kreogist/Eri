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
#include "keportaudiopayload.h"

#include <QDebug>

void KEPortAudioPayload::decodeAndPlay(KEPortAudioStream *streamData,
                                       KEPlaybackBase *playback)
{
    //Get the output data.
    KEAudioBufferData outputBuffer;
    while(streamData->state==PlayingState &&
            streamData->decoder->decodeData(outputBuffer))
    {
        //Check is data empty.
        if(outputBuffer.data.isEmpty())
        {
            //This means that there's no more data we need to play.
            //Reach end of file.
            //Emit finished signal.
            emit playback->finished();
            //Finished!
            break;
        }
        //Update the position, timestamp is the position.
        emit playback->positionChanged(outputBuffer.timestamp);
        //Write it to output device, according to the information.
        PaError writeError=Pa_WriteStream(streamData->stream,
                                          outputBuffer.data.constData(),
                                          outputBuffer.frameCount);
        //Check if there's any error.
        if(writeError!=paNoError)
        {
            qDebug()<<Pa_GetErrorText(writeError);
        }
        //Check the state once more.
        if(streamData->state!=PlayingState)
        {
            if(streamData->state==StoppedState)
            {
                emit playback->positionChanged(0);
            }
            break;
        }
    }
}

KEPortAudioPayload::KEPortAudioPayload(QObject *parent) :
    QObject(parent)
{

}

KEPortAudioPayload::~KEPortAudioPayload()
{

}
