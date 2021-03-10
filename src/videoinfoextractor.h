// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>
#include <QUrl>

#include <ytmusic.h>

class VideoInfoExtractor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString videoId READ videoId WRITE setVideoId NOTIFY videoIdChanged)

    Q_PROPERTY(QUrl audioUrl READ audioUrl NOTIFY audioUrlChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)

public:
    VideoInfoExtractor(QObject *parent = nullptr);

    QUrl audioUrl() const;
    Q_SIGNAL void audioUrlChanged();

    QString videoId() const;
    void setVideoId(const QString &videoId);
    Q_SIGNAL void videoIdChanged();

    QString title() const;
    Q_SIGNAL void titleChanged();

    bool loading() const;
    void setLoading(bool loading);
    Q_SIGNAL void loadingChanged();

    Q_SIGNAL void songChanged();

private:
    bool m_loading = false;
    QString m_videoId;
    video_info::VideoInfo m_videoInfo;
};
