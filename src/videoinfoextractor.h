// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

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

public:
    VideoInfoExtractor(QObject *parent = nullptr);

    QUrl audioUrl() const;
    Q_SIGNAL void audioUrlChanged();

    QString videoId() const;
    void setVideoId(const QString &videoId);
    Q_SIGNAL void videoIdChanged();

    QString title() const;
    Q_SIGNAL void titleChanged();

    Q_SIGNAL void songChanged();

private:
    QString m_videoId;
    video_info::VideoInfo m_videoInfo;
};
