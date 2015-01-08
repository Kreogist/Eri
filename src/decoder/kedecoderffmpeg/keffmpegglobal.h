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
#ifndef KEFFMPEGGLOBAL_H
#define KEFFMPEGGLOBAL_H

//FFMpeg headers
extern "C"
{
#define __STDC_CONSTANT_MACROS
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

#include <QObject>

class KEFfmpegGlobal : public QObject
{
    Q_OBJECT
public:
    static KEFfmpegGlobal *instance();
    ~KEFfmpegGlobal();
    int bufferSize() const;
    int samples() const;
    void setSamples(int samples);
    AVSampleFormat sampleFormat() const;
    void setSampleFormat(const AVSampleFormat &sampleFormat);
    int sampleRate() const;
    void setSampleRate(int sampleRate);
    quint64 channelLayout() const;

signals:

public slots:

private:
    inline void initialFFMpeg();
    inline void resetPatameters();
    inline void updateLayoutParameters();
    explicit KEFfmpegGlobal(QObject *parent = 0);
    static KEFfmpegGlobal *m_instance;

    //Initialized flag.
    bool m_initialized=false;

    //Layout parameters.
    quint64 m_channelLayout;
    AVSampleFormat m_sampleFormat;
    int m_samples;
    int m_sampleRate;

    //Layout information, generate via parameters.
    int m_channels;
    int m_bufferSize;
};

#endif // KEFFMPEGGLOBAL_H
