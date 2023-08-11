// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "videoinfoextractor.h"

#include "asyncytmusic.h"
#include "library.h"
#include "downloadmanager.h"

VideoInfoExtractor::VideoInfoExtractor(QObject *parent)
    : QObject(parent)
{
    connect(this, &VideoInfoExtractor::videoIdChanged, this, [this] {
        if (m_videoId.isEmpty()) {
            m_videoInfo = {};
            Q_EMIT songChanged();
            return;
        }

        setLoading(true);

        QCoro::connect(Library::instance().songDownloaded(m_videoId), this, [this](bool downloaded) {
            m_downloaded = downloaded;
            if (downloaded) {
                Q_EMIT songChanged();
            }

            auto future = YTMusicThread::instance()->extractVideoInfo(QString::fromStdString(m_videoId.toStdString()));
            QCoro::connect(std::move(future), this, [this, downloaded](const video_info::VideoInfo &videoInfo) {
                m_videoInfo = videoInfo;
                setLoading(false);
                if (!downloaded) {
                    Q_EMIT songChanged();
                }
            });
        });
    });
}

QUrl VideoInfoExtractor::audioUrl() const
{
    if (m_downloaded) {
        return QUrl::fromLocalFile(DownloadManager::localPathOf(m_videoId));
    } else {
        return pickAudioUrl(m_videoInfo.formats);
    }
}

QString VideoInfoExtractor::videoId() const
{
    return m_videoId;
}

void VideoInfoExtractor::setVideoId(const QString &videoId)
{
    if (m_videoId == videoId || videoId.isEmpty()) {
        return;
    }
    m_videoId = videoId;
    Q_EMIT videoIdChanged();
}

QString VideoInfoExtractor::title() const
{
    return QString::fromStdString(m_videoInfo.title);
}

QString VideoInfoExtractor::artist() const
{
    return QString::fromStdString(m_videoInfo.artist);
}

QString VideoInfoExtractor::channel() const
{
    return QString::fromStdString(m_videoInfo.channel);
}

QString VideoInfoExtractor::thumbnail() const
{
    return QString::fromStdString(m_videoInfo.thumbnail);
}

bool VideoInfoExtractor::loading() const
{
    return m_loading;
}

void VideoInfoExtractor::setLoading(bool loading)
{
    m_loading = loading;
    Q_EMIT loadingChanged();
}
