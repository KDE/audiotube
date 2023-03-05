// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "clipboard.h"
#include <QClipboard>
#include <QGuiApplication>

Clipboard::Clipboard(QObject *parent)
    : QObject{parent}
{
}

void Clipboard::setText(const QString &text)
{
    QGuiApplication::clipboard()->setText(text);
    Q_EMIT textChanged();
}

QString Clipboard::text() const
{
    return QGuiApplication::clipboard()->text();
}
