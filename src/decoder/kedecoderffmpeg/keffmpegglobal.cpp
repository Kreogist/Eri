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
#include "keffmpegglobal.h"

KEFfmpegGlobal *KEFfmpegGlobal::m_instance=nullptr;

KEFfmpegGlobal::KEFfmpegGlobal(QObject *parent) :
    QObject(parent)
{
    //Initial the ffmpeg library.
    initialFFMpeg();
    //Initial the default output settings.
    resetPatameters();
}

quint64 KEFfmpegGlobal::channelLayout() const
{
    return m_channelLayout;
}

int KEFfmpegGlobal::sampleRate() const
{
    return m_sampleRate;
}

void KEFfmpegGlobal::setSampleRate(int sampleRate)
{
    //Save the sample rate and update parameters.
    m_sampleRate = sampleRate;
    updateLayoutParameters();
}

AVSampleFormat KEFfmpegGlobal::sampleFormat() const
{
    return m_sampleFormat;
}

void KEFfmpegGlobal::setSampleFormat(const AVSampleFormat &sampleFormat)
{
    //Save sample format and update parameters.
    m_sampleFormat = sampleFormat;
    updateLayoutParameters();
}

int KEFfmpegGlobal::samples() const
{
    return m_samples;
}

void KEFfmpegGlobal::setSamples(int samples)
{
    //Save sample and update parameters.
    m_samples = samples;
    updateLayoutParameters();
}

KEFfmpegGlobal *KEFfmpegGlobal::instance()
{
    return m_instance==nullptr?m_instance=new KEFfmpegGlobal:m_instance;
}

KEFfmpegGlobal::~KEFfmpegGlobal()
{
    ;
}

void KEFfmpegGlobal::initialFFMpeg()
{
    //Check the initialized state.
    if(m_initialized)
    {
        return;
    }
    //Initial all muxers, demuxers and protocols of FFMpeg.
    av_register_all();
    //Initial the network components. This is optional, but recommended.
    avformat_network_init();
    //Set the initialized flag.
    m_initialized=true;
}

int KEFfmpegGlobal::bufferSize() const
{
    return m_bufferSize;
}

inline void KEFfmpegGlobal::resetPatameters()
{
    //------This is the default settings------
    //[Locked]Output channel layout.
    //-!This settings cannot be changed.
    m_channelLayout=AV_CH_LAYOUT_STEREO;
    //[AVSampleFormat]The sample mode, default is 16-bit int, base on your CPU.
    m_sampleFormat=AV_SAMPLE_FMT_S16;
    //[Int]The sample size, based on your memory.
    m_samples=1024;
    //[Int]The sample rate.
    m_sampleRate=44100;

    //Update layouts.
    updateLayoutParameters();
}

inline void KEFfmpegGlobal::updateLayoutParameters()
{
    //Get the channels, according to channel layout.
    m_channels=av_get_channel_layout_nb_channels(m_channelLayout);
    //Update the buffer size.
    m_bufferSize=av_samples_get_buffer_size(NULL,
                                            m_channels,
                                            m_samples,
                                            m_sampleFormat,
                                            1); //1 == no alignment
}
