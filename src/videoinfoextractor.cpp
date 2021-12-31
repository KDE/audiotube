// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "videoinfoextractor.h"

#include <QFutureWatcher>

#include "asyncytmusic.h"
#include "library.h"

VideoInfoExtractor::VideoInfoExtractor(QObject *parent)
    : QObject(parent)
{
    connect(this, &VideoInfoExtractor::videoIdChanged, this, [this] {
        if (m_videoId.isEmpty()) {
            m_videoInfo = {};
            Q_EMIT audioUrlChanged();
            Q_EMIT titleChanged();
            Q_EMIT songChanged();
            Q_EMIT thumbnailChanged();
            return;
        }

        setLoading(true);

        auto future = YTMusicThread::instance()->extractVideoInfo(QString::fromStdString(m_videoId.toStdString()));
        connectFuture(future, this, [this](const video_info::VideoInfo &videoInfo) {
            m_videoInfo = videoInfo;
            setLoading(false);
            Library::instance().addVideoTitle(m_videoId, QString::fromStdString(m_videoInfo.title));
            Q_EMIT audioUrlChanged();
            Q_EMIT titleChanged();
            Q_EMIT songChanged();
            Q_EMIT thumbnailChanged();
        });
    });
}

QUrl VideoInfoExtractor::audioUrl() const
{
    if (m_videoInfo.formats.empty()) {
        return {};
    }

    std::vector<video_info::Format> audioFormats;

    // filter audio only formats
    std::copy_if(m_videoInfo.formats.begin(), m_videoInfo.formats.end(), std::back_inserter(audioFormats),
        [](const video_info::Format &format) {
        return format.acodec != "none" && format.vcodec == "none";
    });

    if (audioFormats.empty()) {
        return {};
    }

    std::sort(audioFormats.begin(), audioFormats.end(),
              [](const video_info::Format &a, const video_info::Format &b) {
        return a.quality > b.quality;
    });

    return QUrl(QString::fromStdString(audioFormats.front().url));
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
