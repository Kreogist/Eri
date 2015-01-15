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
#ifndef KEDECODERFFMPEG_H
#define KEDECODERFFMPEG_H

#include <QFileInfo>

#include "keffmpegglobal.h"

#include "kedecoderbase.h"

struct AVFormatContext;
struct AVPacket;
struct AVCodec;
struct AVCodecContext;
struct AVFrame;
struct SwrContext;
class KEFfmpegGlobal;
class KEDecoderFfmpeg : public KEDecoderBase
{
    Q_OBJECT
public:
    explicit KEDecoderFfmpeg(QObject *parent = 0);
    ~KEDecoderFfmpeg();
    bool reset();
    bool loadLocalFile(const QString &filePath);
    bool decodeData(KEAudioBufferData &buffer);
    int state();
    int sampleRate();
    int duration();
    quint64 channelLayout() const;
    int sampleFormat() const;

public slots:
    bool seek(const qint64 &position);

private:
    bool parseFormatContext();
    inline void setSampleFormat(const AVSampleFormat &sampleFormat);
    KEFfmpegGlobal *m_ffmpegGlobal;
    SwrContext *m_resampleContext=nullptr;
    AVFormatContext *m_formatContext=nullptr;
    AVCodecContext *m_codecContext=nullptr;
    AVCodec *m_codec=nullptr;
    quint8 **m_audioBuffer=nullptr;
    QFileInfo m_currentFileInfo;
    int m_audioStreamIndex=-1;
    int m_audioFrameSize=0;
    int m_sampleRate=0;
    quint64 m_channelLayout=0;
    AVSampleFormat m_sampleFormat;
    int m_destinationSamples;
    double m_timeBase=1.0;
};

#endif // KEDECODERFFMPEG_H
