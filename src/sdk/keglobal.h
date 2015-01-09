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
#ifndef KEGLOBAL_H
#define KEGLOBAL_H

#include <QObject>

namespace Eri
{
struct BufferData
{
    int timestamp;
    int frameCount;
    QByteArray data;
};
enum KESampleFormat
{
    SampleUnsignInt8,
    SampleSignInt16,
    SampleSignInt32,
    SampleFloat,
    SampleDouble
};
}

using namespace Eri;

class KEGlobal : public QObject
{
    Q_OBJECT
public:
    static KEGlobal *instance();
    ~KEGlobal();
    int sampleFormat() const;
    void setSampleFormat(int sampleFormat);

signals:
    void updateResample();

public slots:

private:
    static KEGlobal *m_instance;
    explicit KEGlobal(QObject *parent = 0);
    int m_sampleFormat;
};

#endif // KEGLOBAL_H
