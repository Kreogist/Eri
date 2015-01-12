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
#include <QDir>

#include "keffmpegglobal.h"
#include "kedecoderffmpeg.h"

#include <QDebug>

KEDecoderFfmpeg::KEDecoderFfmpeg(QObject *parent) :
    KEDecoderBase(parent)
{
    //Initial the ffmpeg, get ffmpeg global.
    m_ffmpegGlobal=KEFfmpegGlobal::instance();

    //Generate buffer, set the default audio frame size.
    //The 192000 means 1 second of 32bits 48kHz audio.
    //To calculate this: means 4bytes(32bits)*48000=192000.
    m_audioFrameSize=192000;
    //Reset decoder.
    reset();
}

KEDecoderFfmpeg::~KEDecoderFfmpeg()
{
    //Reset the decoder, release the memory.
    reset();
    //Free the format context memory.
    avformat_free_context(m_formatContext);
    //Free the audio buffer.
    if(m_audioBuffer[0])
    {
        av_freep(&m_audioBuffer[0]);
    }
    av_freep(&m_audioBuffer);
}

bool KEDecoderFfmpeg::reset()
{
    //Check the format context state.
    if(m_formatContext!=nullptr)
    {
        //Close the format context.
        avformat_close_input(&m_formatContext);
        //Release the format context to NULL.
        m_formatContext=nullptr;
    }
    //Reset the file info.
    m_currentFileInfo=QFileInfo();
    //Initial the audio stream.
    m_audioStreamIndex=-1;
    //Reset codec context and decoder;
    m_codecContext=nullptr;
    m_codec=nullptr;
    //Check the resample context data.
    if(m_resampleContext!=nullptr)
    {
        //Free the resample context.
        swr_free(&m_resampleContext);
        //FFMpeg should done this.
        m_resampleContext=nullptr;
    }
    return true;
}

bool KEDecoderFfmpeg::loadLocalFile(const QString &filePath)
{
    //Reset the thread.
    reset();
    //Check the file is exist or not.
    QFileInfo fileCheck(filePath);
    if(!fileCheck.exists())
    {
        qDebug()<<"File not exist.";
        //!FIXME: Set error to "File not exist."
        return false;
    }
    //Save the file info.
    m_currentFileInfo=fileCheck;

    //Load the file.
    std::string stdFilePath=QDir::toNativeSeparators(fileCheck.absoluteFilePath()).toStdString();
    //Initial the decoder format context.
    m_formatContext=avformat_alloc_context();
    //Open the format context.
    if(avformat_open_input(&m_formatContext,
                           stdFilePath.data(),
                           NULL,
                           NULL)!=0)
    {
        qDebug()<<"Cannot open file.";
        //!FIXME: Set error to "Cannot open file."
        return false;
    }
    return parseFormatContext();
}

KEAudioBufferData KEDecoderFfmpeg::decodeData()
{
    //Check format context first.
    if(m_formatContext==nullptr)
    {
        return KEAudioBufferData();
    }
    //Initial a packet and a frame.
    AVFrame *audioFrame=av_frame_alloc();
    AVPacket packet;
    //Find the next audio stream frame.
    av_init_packet(&packet);
    while(av_read_frame(m_formatContext, &packet)>=0)
    {
        //Ensure that the stream is audio stream.
        if(packet.stream_index==m_audioStreamIndex)
        {
            //Decode the audio.
            int gotFramePointer=0, decodeResult=0;
            //Decode the audio data until the packet size.
            while(decodeResult!=packet.size)
            {
                decodeResult=avcodec_decode_audio4(m_codecContext,
                                                   audioFrame,
                                                   &gotFramePointer,
                                                   &packet);
            }
            //Ignore the no used frame.
            if(gotFramePointer>0)
            {
                int current_nb_samples=av_rescale_rnd(swr_get_delay(m_resampleContext, audioFrame->sample_rate) + audioFrame->nb_samples,
                                                      m_ffmpegGlobal->sampleRate(),
                                                      audioFrame->sample_rate,
                                                      AV_ROUND_UP);
                int dst_linesize;
                if(current_nb_samples>dst_nb_samples)
                {
                    av_free(m_audioBuffer[0]);
                    av_samples_alloc(m_audioBuffer,
                                     &dst_linesize,
                                     av_get_channel_layout_nb_channels(m_ffmpegGlobal->channelLayout()),
                                     current_nb_samples,
                                     m_ffmpegGlobal->sampleFormat(),
                                     1);
                    dst_nb_samples=current_nb_samples;
                }
                //Resampling the data.
                int ret=swr_convert(m_resampleContext,
                                    m_audioBuffer,
                                    dst_nb_samples,
                                    (const quint8 **)audioFrame->data,
                                    audioFrame->nb_samples);
                //Generate the buffer data.
                KEAudioBufferData buffer;
                //The frame count.
                buffer.frameCount=audioFrame->nb_samples;
                //First frame's timestamp.
                buffer.timestamp=packet.pts*m_timeBase;
                //Buffer raw data.
                buffer.data=QByteArray((char *)m_audioBuffer[0],
                                       av_samples_get_buffer_size(&dst_linesize,
                                                                  av_get_channel_layout_nb_channels(m_ffmpegGlobal->channelLayout()),
                                                                  ret,
                                                                  m_ffmpegGlobal->sampleFormat(),
                                                                  1));
                //Free the packet and frame.
                av_free_packet(&packet);
                av_frame_free(&audioFrame);
                //Return the data.
                return buffer;
            }
        }
        av_free_packet(&packet);
    }
    av_free_packet(&packet);
    av_frame_free(&audioFrame);
    return KEAudioBufferData();
}

int KEDecoderFfmpeg::state()
{
    ;
}

int KEDecoderFfmpeg::bufferSize()
{
    return m_ffmpegGlobal->bufferSize();
}

int KEDecoderFfmpeg::sampleRate()
{
    return m_ffmpegGlobal->sampleRate();
}

int KEDecoderFfmpeg::duration()
{
    return m_formatContext==nullptr?-1:m_formatContext->duration/(AV_TIME_BASE/1000);
}

bool KEDecoderFfmpeg::seek(const qint64 &position)
{
    return (av_seek_frame(m_formatContext, m_audioStreamIndex, position, 0)>=0);
}

bool KEDecoderFfmpeg::parseFormatContext()
{
    //Get the stream information.
    if(avformat_find_stream_info(m_formatContext, NULL)<0)
    {
        qDebug()<<"Cannot find stream information.";
        //!FIXME: Set error to "Cannot find stream information."
        return false;
    }

    //Find the audio stream.
    for(unsigned int i=0; i<m_formatContext->nb_streams; i++)
    {
        //Find the audio stream.
        if(m_formatContext->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO)
        {
            //Save the stream index.
            m_audioStreamIndex=i;
            //Save the time base.
            m_timeBase=av_q2d(m_formatContext->streams[m_audioStreamIndex]->time_base)*1000.0;
            break;
        }
    }
    //Return false if there's no audio stream.
    if(-1==m_audioStreamIndex)
    {
        qDebug()<<"Cannot find audio stream.";
        //!FIXME: Set error to "Cannot find audio stream."
        return false;
    }

    //Find the decoder.
    m_codecContext=m_formatContext->streams[m_audioStreamIndex]->codec;
    m_codec=avcodec_find_decoder(m_codecContext->codec_id);
    if(NULL==m_codec)
    {
        qDebug()<<"Cannot find decoder.";
        //!FIXME: Set error to "Cannot find decoder."
        return false;
    }
    //Open the codec context via decoder.
    if(avcodec_open2(m_codecContext, m_codec, NULL)<0)
    {
        qDebug()<<"Cannot open the decoder.";
        //!FIXME: Set error to "Cannot open the decoder."
        return false;
    }

//    //Initial the audio resampling context.
//    m_resampleContext=swr_alloc();
    //Update the resampling data.
    m_resampleContext=swr_alloc_set_opts(m_resampleContext,
                                         m_ffmpegGlobal->channelLayout(),
                                         m_ffmpegGlobal->sampleFormat(),
                                         m_ffmpegGlobal->sampleRate(),
                                         av_get_default_channel_layout(m_codecContext->channels),
                                         m_codecContext->sample_fmt,
                                         m_codecContext->sample_rate,
                                         0,
                                         NULL);
    //Initial the audio resampling.
    swr_init(m_resampleContext);
    //Update output sample numbers.
    dst_nb_samples=av_rescale_rnd(1024,
                                  m_ffmpegGlobal->sampleRate(),
                                  m_codecContext->sample_rate,
                                  AV_ROUND_UP);
    int dst_linesize;
    int ret=av_samples_alloc_array_and_samples(&m_audioBuffer,
                                               &dst_linesize,
                                               av_get_channel_layout_nb_channels(m_ffmpegGlobal->channelLayout()),
                                               dst_nb_samples,
                                               m_ffmpegGlobal->sampleFormat(),
                                               0);
    if(ret<0)
    {
        qDebug()<<"Could not allocate destination samples.";
    }
    return true;
}

