// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "playerutils.h"

PlayerUtils::PlayerUtils(QObject *parent) : QObject(parent)
{

}

QString PlayerUtils::formatTimestamp(quint64 stamp)
{
    return m_format.formatDuration(stamp, KFormat::FoldHours);
}
