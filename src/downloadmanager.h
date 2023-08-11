// SPDX-FileCopyrightText: 2023 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>

#include <QCoroTask>
#include <QCoroQmlTask>

class DownloadManager : public QObject
{
    Q_OBJECT

public:
    explicit DownloadManager(QObject *parent = nullptr);
    Q_INVOKABLE QCoro::QmlTask downloadSong(const QString &videoId);
    Q_INVOKABLE void deleteDownload(const QString &videoId);

    static QString localPathOf(const QString &videoId);

private:
    QCoro::Task<> download(const QString videoId);

    static QString downloadDirectory();
};

class DownloadedWatcher : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString videoId MEMBER m_videoId NOTIFY videoIdChanged)
    Q_PROPERTY(bool downloaded MEMBER m_downloaded NOTIFY downloadedChanged)

public:
    explicit DownloadedWatcher(QObject *parent = nullptr);

    Q_SIGNAL void downloadedChanged();
    Q_SIGNAL void videoIdChanged();

private:
    bool m_downloaded = false;
    QString m_videoId;
};
