// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>
#include <QUrl>

#include <ytmusic.h>

class VideoInfoExtractor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString videoId READ videoId WRITE setVideoId NOTIFY videoIdChanged)

    Q_PROPERTY(QUrl audioUrl READ audioUrl NOTIFY songChanged)
    Q_PROPERTY(QString title READ title NOTIFY songChanged)
    Q_PROPERTY(QString artist READ artist NOTIFY songChanged) // may be empty if it is a video
    Q_PROPERTY(QString channel READ channel NOTIFY songChanged)
    Q_PROPERTY(QString thumbnail READ thumbnail NOTIFY songChanged)
    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY songChanged)

public:
    explicit VideoInfoExtractor(QObject *parent = nullptr);

    QUrl audioUrl() const;

    QString videoId() const;
    void setVideoId(const QString &videoId);
    Q_SIGNAL void videoIdChanged();

    QString title() const;

    QString artist() const;

    QString channel() const;

    QString thumbnail() const;

    bool loading() const;
    void setLoading(bool loading);
    Q_SIGNAL void loadingChanged();

    Q_SIGNAL void songChanged();

private:
    bool m_loading = false;
    bool m_downloaded = false;
    QString m_videoId;
    video_info::VideoInfo m_videoInfo;
};
