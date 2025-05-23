// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>
#include <qqmlintegration.h>

#include <KFormat>

class PlayerUtils : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit PlayerUtils(QObject *parent = nullptr);

    Q_INVOKABLE QString formatTimestamp(quint64 stamp);
    Q_INVOKABLE float convertVolume(float volume);

private:
    KFormat m_format;
};

