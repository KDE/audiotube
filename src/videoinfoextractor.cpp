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
    connect(&m_thumbnailSource, &ThumbnailSource::cachedPathChanged, this, &VideoInfoExtractor::songChanged);

    connect(this, &VideoInfoExtractor::videoIdChanged, this, [this] {
        qDebug() << "video id changed to" << m_videoId;
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

            if (downloaded) {
                m_thumbnailSource.setVideoId(m_videoId);

                QCoro::connect(Library::instance().getSong(m_videoId), this, [this](auto &&song) {
                    if (song) {
                        m_localInfo = std::move(*song);
                        setLoading(false);
                        Q_EMIT songChanged();
                    }
                });
            }

            auto future = YTMusicThread::instance()->extractVideoInfo(QString::fromStdString(m_videoId.toStdString()));
            QCoro::connect(std::move(future), this, [this](video_info::VideoInfo &&videoInfo) {
                if (videoInfo.id.empty()) {
                    return;
                }
                m_videoInfo = std::move(videoInfo);
                setLoading(false);
                Q_EMIT songChanged();
            });
        });
    });
}

QUrl VideoInfoExtractor::audioUrl() const
{
    if (m_videoId.isEmpty()) {
        return {};
    }

    if (!m_videoInfo) {
        return QUrl::fromLocalFile(DownloadManager::localPathOf(m_videoId));
    } else {
        return pickAudioUrl(m_videoInfo->formats);
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
    if (!m_videoInfo) {
        return m_localInfo.title;
    }
    return QString::fromStdString(m_videoInfo->title);
}

QString VideoInfoExtractor::artist() const
{
    if (!m_videoInfo) {
        return m_localInfo.artist;
    }
    return QString::fromStdString(m_videoInfo->artist);
}

QUrl VideoInfoExtractor::thumbnail() const
{
    if (!m_videoInfo) {
        return m_thumbnailSource.cachedPath();
    }
    return QUrl(QString::fromStdString(m_videoInfo->thumbnail));
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
