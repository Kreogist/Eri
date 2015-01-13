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
#ifndef KEOPENALGLOBAL_H
#define KEOPENALGLOBAL_H

#include <AL/al.h>
#include <AL/alc.h>

#include <QObject>

class KEGlobal;
class KEOpenALGlobal : public QObject
{
    Q_OBJECT
public:
    static KEOpenALGlobal *instance();
    ~KEOpenALGlobal();
    bool isAvailable();
    ALCdevice *currentDevice();
    ALenum outputFormat();
    int sampleRate() const;

signals:

public slots:

private slots:
    void onActionResampleUpdate();

private:
    inline void initialAudioDevice();
    static KEOpenALGlobal *m_instance;
    explicit KEOpenALGlobal(QObject *parent = 0);
    KEGlobal *m_global;

    //Initialized flag.
    bool m_available=false;
    bool m_initialized=false;

    //Available devices.
    QVector<QByteArray> m_devices;
    ALCdevice *m_currentDevice;

    //Output format.
    ALenum m_format=AL_FORMAT_STEREO16;
};

#endif // KEOPENALGLOBAL_H
