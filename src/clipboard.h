// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <QObject>
#include <qqmlintegration.h>

class Clipboard : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

public:
    explicit Clipboard(QObject *parent = nullptr);

    void setText(const QString &text);
    QString text() const;

    Q_SIGNAL void textChanged();
};

#endif // CLIPBOARD_H
