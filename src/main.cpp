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
#include <QApplication>
#include <QMainWindow>

#include "decoder/kedecoderffmpeg/kedecoderffmpeg.h"

#include "playback/keplaybackportaudio/keplaybackportaudio.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QMainWindow mainWindow;

    KEDecoderBase *ffmpegDecoder=new KEDecoderFfmpeg;
    qDebug()<<ffmpegDecoder->loadLocalFile("D:\\音乐\\Compact Discs\\ラブライブ！\\[100825] ラブライブ！ 1stシングル「僕らのLIVE 君とのLIFE」／μ's (FLAC+BK)\\01. 僕らのLIVE 君とのLIFE.flac");
//    qDebug()<<ffmpegDecoder->loadLocalFile("D:\\音乐\\Compact Discs\\Various.Artists.-.[Essential.Bach.36.Greatest.Masterpieces.2CD.DECCA].专辑.(APE)\\Essential Bach CD2\\01. Brandenburg Concerto No. 2 in F major, BWV 1047.m4a");
    KEPlaybackBase *portAudioPlayback=new KEPlaybackPortAudio;
    portAudioPlayback->setDecoder(ffmpegDecoder);
    delete portAudioPlayback;
    delete ffmpegDecoder;
    mainWindow.show();
    return app.exec();
}
