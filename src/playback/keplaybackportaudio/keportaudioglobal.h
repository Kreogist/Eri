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
#ifndef KEPORTAUDIOGLOBAL_H
#define KEPORTAUDIOGLOBAL_H

#include <QHash>

#include "portaudio.h"

#include <QObject>

class KEGlobal;
class KEPortAudioGlobal : public QObject
{
    Q_OBJECT
public:
    static KEPortAudioGlobal *instance();
    ~KEPortAudioGlobal();
    bool isAvailable();
    PaStreamParameters *outputParameters();
    int sampleFormat() const;
    int sampleRate() const;
    int outputChannels() const;

signals:
    void requireUpdateResample();

public slots:

private slots:
    void onActionResampleUpdate();

private:
    inline void initialPortAudio();
    inline void initialAudioDevice();
    static KEPortAudioGlobal *m_instance;
    explicit KEPortAudioGlobal(QObject *parent = 0);
    //Initialized flag.
    bool m_available=false;
    bool m_initialized=false;

    PaStreamParameters m_outputParameters;
    KEGlobal *m_global;
    QHash<int, int> m_sampleFormatMap;
    int m_sampleFormat;
};

#endif // KEPORTAUDIOGLOBAL_H
