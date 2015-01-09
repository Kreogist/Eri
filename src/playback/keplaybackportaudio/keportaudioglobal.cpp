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
#include "keglobal.h"

#include "keportaudioglobal.h"

KEPortAudioGlobal *KEPortAudioGlobal::m_instance=nullptr;

KEPortAudioGlobal *KEPortAudioGlobal::instance()
{
    return m_instance==nullptr?m_instance=new KEPortAudioGlobal:m_instance;
}

KEPortAudioGlobal::KEPortAudioGlobal(QObject *parent) :
    QObject(parent)
{
    //Initial the PortAudio.
    initialPortAudio();
    //If initialize PortAudio successfully, then load the device data.
    if(m_available)
    {
        //Initial the KEGlobal.
        m_global=KEGlobal::instance();
        //Initial output parameters.
        initialAudioDevice();
        //Initial the sample format map.
        m_sampleFormatMap.insert(SampleUnsignInt8, paUInt8);
        m_sampleFormatMap.insert(SampleSignInt16, paInt16);
        m_sampleFormatMap.insert(SampleSignInt32, paInt32);
        m_sampleFormatMap.insert(SampleFloat, paFloat32);
        //Link the update request.
        connect(m_global, &KEGlobal::updateResampleConfigure,
                this, &KEPortAudioGlobal::onActionResampleUpdate);
        //Update the resample settings.
        onActionResampleUpdate();
    }
}

int KEPortAudioGlobal::sampleFormat() const
{
    return m_sampleFormat;
}

int KEPortAudioGlobal::sampleRate() const
{
    return m_global->sampleRate();
}

int KEPortAudioGlobal::outputChannels() const
{
    return m_global->channel();
}

KEPortAudioGlobal::~KEPortAudioGlobal()
{
    ;
}

bool KEPortAudioGlobal::isAvailable()
{
    return m_available;
}

PaStreamParameters *KEPortAudioGlobal::outputParameters()
{
    return &m_outputParameters;
}

void KEPortAudioGlobal::onActionResampleUpdate()
{
    //Update the sample format.
    m_sampleFormat=m_sampleFormatMap.value(m_global->sampleFormat());
    //Emit update signal.
    emit requireUpdateResample();
}

inline void KEPortAudioGlobal::initialPortAudio()
{
    //Check the initialized state.
    if(m_initialized)
    {
        return;
    }
    //Initial the PortAudio.
    m_available=(paNoError==Pa_Initialize());
    //Set the initialized flag.
    m_initialized=true;
}

void KEPortAudioGlobal::initialAudioDevice()
{
    //Reset the output parameters.
    m_outputParameters.device=Pa_GetDefaultOutputDevice();
    //Check if we can get the audio device.
    if(m_outputParameters.device==paNoDevice)
    {
        //No device can use, return false.
        m_available=false;
        return;
    }
    //Reset the output parameters.
    m_outputParameters.hostApiSpecificStreamInfo=NULL;
    m_outputParameters.suggestedLatency=Pa_GetDeviceInfo(m_outputParameters.device)->defaultHighOutputLatency;
}

