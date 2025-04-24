// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>
#include <qqmlintegration.h>

#include "asyncytmusic.h"

class ErrorHandler : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit ErrorHandler(QObject *parent = nullptr);

    Q_SIGNAL void errorOccurred(const QString &error);
};
