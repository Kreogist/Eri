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

#include "keopenalglobal.h"

KEOpenALGlobal::KEOpenALGlobal(QObject *parent) :
    QObject(parent)
{
    //Initial global.
    m_global=KEGlobal::instance();
    //Initial OpenAL.
    if(!m_initialized)
    {
        //Initial the devices.
        initialAudioDevice();
        //Link the update request.
        connect(m_global, &KEGlobal::updateResampleConfigure,
                this, &KEOpenALGlobal::onActionResampleUpdate);
        //Initial the output format, using updater.
        onActionResampleUpdate();
        m_initialized=true;
    }
}

KEOpenALGlobal *KEOpenALGlobal::m_instance=nullptr;

KEOpenALGlobal *KEOpenALGlobal::instance()
{
    return m_instance==nullptr?m_instance=new KEOpenALGlobal:m_instance;
}

KEOpenALGlobal::~KEOpenALGlobal()
{

}

bool KEOpenALGlobal::isAvailable()
{
    return m_available;
}

ALCdevice *KEOpenALGlobal::currentDevice()
{
    return m_currentDevice;
}

ALenum KEOpenALGlobal::outputFormat()
{
    return m_format;
}

int KEOpenALGlobal::sampleRate() const
{
    return m_global->sampleRate();
}

void KEOpenALGlobal::onActionResampleUpdate()
{
    //QtAV: A context is required for al functions!
    //I don't know what fuck this is, it seems important.
    ALCcontext *context=alcGetCurrentContext();
    ALenum format=0;
    switch(m_global->sampleFormat())
    {
    case SampleUnsignInt8:
        //Because the channel has been locked at 2. So this must be stero.
        format=AL_FORMAT_STEREO8;
        break;
    case SampleSignInt16:
        format=AL_FORMAT_STEREO16;
        break;
    case SampleSignInt32:
        //No 32-bit signed integer support in OpenAL.
        break;
    case SampleFloat:
        if(context && alIsExtensionPresent("AL_EXT_float32"))
        {
            format=alGetEnumValue("AL_FORMAT_STEREO_FLOAT32");
        }
        break;
    case SampleDouble:
        if(context && alIsExtensionPresent("AL_EXT_double"))
        {
            format=alGetEnumValue("AL_FORMAT_STEREO_DOUBLE_EXT");
        }
        break;
    }
    //Check is the new format available.
    if(format!=0)
    {
        //Do updates.
        m_format=format;
    }
}

inline void KEOpenALGlobal::initialAudioDevice()
{
    //Get all the available devices.
    const char *rawDeviceData=nullptr;
#ifdef ALC_ALL_DEVICES_SPECIFIER
    rawDeviceData=alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
#else
    if(alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT"))
    {
        // avoid using enum ALC_ALL_DEVICES_SPECIFIER directly
        rawDeviceData=alcGetString(NULL,
                                   alcGetEnumValue(NULL, "ALC_ALL_DEVICES_SPECIFIER"));
    }
    else
    {
        rawDeviceData=alcGetString(NULL, ALC_DEVICE_SPECIFIER);
    }
#endif
    while(rawDeviceData && *rawDeviceData) //Magic, don't touch.
    {
        m_devices.push_back(rawDeviceData);
        rawDeviceData+=m_devices.last().size()+1;
    }
    //Get the default device, used as current device.
    //Why such a lib using a string as identifier = =|||
    m_currentDevice=alcOpenDevice(alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER));
}
