/*
 * SPDX-FileCopyrightText: 2020-2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <QObject>
#include <QQuickItem>
#include <qqmlintegration.h>

class Blur : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    Blur(QObject *parent = nullptr);
    Q_INVOKABLE void setBlur(QQuickItem *item, bool blur);
};
