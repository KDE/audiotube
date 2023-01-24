// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "playerutils.h"

 #include <QAudio>

PlayerUtils::PlayerUtils(QObject *parent) : QObject(parent)
{

}

QString PlayerUtils::formatTimestamp(quint64 stamp)
{
    return m_format.formatDuration(stamp, KFormat::FoldHours);
}

float PlayerUtils::convertVolume(float volume)
{
    return QAudio::convertVolume(volume, QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
}
