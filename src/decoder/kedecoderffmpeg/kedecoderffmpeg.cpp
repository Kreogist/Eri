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

#define MAX_AUDIO_FRAME_SIZE 192000

KEDecoderFfmpeg::KEDecoderFfmpeg(QObject *parent) :
    KEDecoderBase(parent)
{
    //Initial the ffmpeg, get ffmpeg global.
    m_ffmpegGlobal=KEFfmpegGlobal::instance();

    //Initial the decoder format context.
    m_formatContext=avformat_alloc_context();
    //Initial the packet.
    m_packet=(AVPacket *)malloc(sizeof(AVPacket));
    av_init_packet(m_packet);
    //Initial the frame.
    m_audioFrame=av_frame_alloc();
    //Generate buffer.
    m_audioBuffer=(quint8 *)av_malloc(MAX_AUDIO_FRAME_SIZE*2);
    //Reset decoder.
    reset();
}

KEDecoderFfmpeg::~KEDecoderFfmpeg()
{
    //Reset the decoder, release the memory.
    reset();
    //Free the packet memory.
    delete m_packet;
    //Free the format context memory.
    avformat_free_context(m_formatContext);
    //Free the frame memory.
    av_frame_free(&m_audioFrame);
    //Free the audio buffer.
    av_free(m_audioBuffer);
}

bool KEDecoderFfmpeg::reset()
{
    //Reset the file info.
    m_currentFileInfo=QFileInfo();
    //Initial the audio stream.
    m_audioStreamIndex=-1;
    //Reset codec context;
    m_codecContext=NULL;
    //Check the resample context data.
    if(m_resampleContext!=NULL)
    {
        //Free the resample context.
        swr_free(&m_resampleContext);
        //FFMpeg should done this.
        m_resampleContext=NULL;
    }
    //Free the packet.
    av_free_packet(m_packet);
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
        //!FIXME: Set error to "File not exist."
        return false;
    }
    //Save the file info.
    m_currentFileInfo=fileCheck;

    //Load the file.
    std::string stdFilePath=QDir::toNativeSeparators(fileCheck.absoluteFilePath()).toStdString();
    if(avformat_open_input(&m_formatContext,
                           stdFilePath.data(),
                           NULL,
                           NULL)!=0)
    {
        //!FIXME: Set error to "Cannot open file."
        return false;
    }
    return parseFormatContext();
}

KEAudioBufferData KEDecoderFfmpeg::decodeData()
{
    //Find the next audio stream frame.
    while(av_read_frame(m_formatContext, m_packet)>=0)
    {
        //Ensure that the stream is audio stream.
        if(m_packet->stream_index==m_audioStreamIndex)
        {
            //Decode the audio.
            int gotFramePointer=0,
                decodeResult=avcodec_decode_audio4(m_codecContext,
                                                   m_audioFrame,
                                                   &gotFramePointer,
                                                   m_packet);
            //Check if we decode it successful.
            if(decodeResult>=0 && gotFramePointer>0)
            {
                //Resampling the data.
                swr_convert(m_resampleContext,
                            &m_audioBuffer,
                            MAX_AUDIO_FRAME_SIZE,
                            (const quint8 **)m_audioFrame->data,
                            m_audioFrame->nb_samples);
                //Generate the buffer data.
                KEAudioBufferData buffer;
                //The frame count.
                buffer.frameCount=m_audioFrame->nb_samples;
                //First frame's timestamp.
                buffer.timestamp=m_audioFrame->pkt_pts;
                //Buffer raw data.
                buffer.data=QByteArray((char *)m_audioBuffer,
                                       MAX_AUDIO_FRAME_SIZE);
                //Free the packet.
                av_free_packet(m_packet);
                //Return the data.
                return buffer;
            }
        }
        av_free_packet(m_packet);
    }
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

bool KEDecoderFfmpeg::seek(const qint64 &position)
{
    return (av_seek_frame(m_formatContext, m_audioStreamIndex, position, 0)>=0);
}

bool KEDecoderFfmpeg::parseFormatContext()
{
    //Get the stream information.
    if(avformat_find_stream_info(m_formatContext, NULL)<0)
    {
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
            break;
        }
    }
    //Return false if there's no audio stream.
    if(-1==m_audioStreamIndex)
    {
        //!FIXME: Set error to "Cannot find audio stream."
        return false;
    }

    //Find the decoder.
    m_codecContext=m_formatContext->streams[m_audioStreamIndex]->codec;
    m_decoder=avcodec_find_decoder(m_codecContext->codec_id);
    if(NULL==m_decoder)
    {
        //!FIXME: Set error to "Cannot find decoder."
        return false;
    }
    //Open the codec context via decoder.
    if(avcodec_open2(m_codecContext, m_decoder, NULL)<0)
    {
        //!FIXME: Set error to "Cannot open the decoder."
        return false;
    }

    //Initial the audio resampling context.
    m_resampleContext=swr_alloc();
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
    return true;
}

