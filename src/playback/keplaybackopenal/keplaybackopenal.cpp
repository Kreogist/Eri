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
#include "keplaybackopenal.h"

#include <QDebug>

KEPlaybackOpenAL::KEPlaybackOpenAL(QObject *parent) :
    KEPlaybackBase(parent)
{
    m_openALGlobal=KEOpenALGlobal::instance();
    //Create context on the current device.
    context = alcCreateContext(m_openALGlobal->currentDevice(),
                               NULL);
    alcMakeContextCurrent(context);
    //Generate buffers.
    alGenBuffers(kBufferCount, buffer);
    //Generate source. (WTF...)
    alGenSources(1, source);
    //Set source property.
    alSourcei(source, AL_LOOPING, AL_FALSE);
    alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
    alSourcei(source, AL_ROLLOFF_FACTOR, 0);
    alSource3f(source, AL_POSITION, 0.0, 0.0, 0.0);
    alSource3f(source, AL_VELOCITY, 0.0, 0.0, 0.0);
    alListener3f(AL_POSITION, 0.0, 0.0, 0.0);
    //Initial all buffers.
    static char init_data[kBufferSize];
    memset(init_data, 0, sizeof(init_data));
    for (int i = 1; i < kBufferCount; ++i)
    {
        AL_RUN_CHECK(alBufferData(buffer[i],
                                  m_openALGlobal->outputFormat(),
                                  init_data,
                                  sizeof(init_data),
                                  m_openALGlobal->sampleRate()));
        AL_RUN_CHECK(alSourceQueueBuffers(source,
                                          1,
                                          &buffer[i]));
//        d.nextEnqueueInfo().data_size = sizeof(init_data);
//        d.nextEnqueueInfo().timestamp = 0;
//        d.bufferAdded();
    }
}

KEPlaybackOpenAL::~KEPlaybackOpenAL()
{

}

void KEPlaybackOpenAL::reset()
{
    ;
}

bool KEPlaybackOpenAL::setDecoder(KEDecoderBase *decoder)
{
    ;
}

void KEPlaybackOpenAL::start()
{
    ;
}

void KEPlaybackOpenAL::pause()
{
    ;
}

void KEPlaybackOpenAL::stop()
{
    ;
}

int KEPlaybackOpenAL::state() const
{
    ;
}

