// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>

#include "asyncytmusic.h"

class ErrorHandler : public QObject
{
    Q_OBJECT

public:
    explicit ErrorHandler(QObject *parent = nullptr);

    Q_SIGNAL void errorOccurred(const QString &error);
};
