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
#ifndef KEPLAYBACKOPENAL_H
#define KEPLAYBACKOPENAL_H

#include "keopenalglobal.h"

#include "keplaybackbase.h"

class KEPlaybackOpenAL : public KEPlaybackBase
{
    Q_OBJECT
public:
    explicit KEPlaybackOpenAL(QObject *parent = 0);
    ~KEPlaybackOpenAL();
    void reset();
    bool setDecoder(KEDecoderBase *decoder);
    void start();
    void pause();
    void stop();
    int state() const;

private:
    const int kBufferSize = 1024*4;
    const int kBufferCount = 8;
    KEOpenALGlobal *m_openALGlobal;
    ALCcontext *context;
    ALuint buffer[8]; //Don't ask me why!
    ALuint source;
    ALint state;
};

#endif // KEPLAYBACKOPENAL_H
