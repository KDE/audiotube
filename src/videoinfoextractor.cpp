// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "videoinfoextractor.h"

#include "asyncytmusic.h"

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

        auto future = YTMusicThread::instance()->extractVideoInfo(QString::fromStdString(m_videoId.toStdString()));
        QCoro::connect(std::move(future), this, [this](const video_info::VideoInfo &videoInfo) {
            m_videoInfo = videoInfo;
            setLoading(false);
            Q_EMIT songChanged();
        });
    });
}

QUrl VideoInfoExtractor::audioUrl() const
{
    if (m_videoInfo.formats.empty()) {
        qWarning() << "No formats found";
        return {};
    }

    std::vector<video_info::Format> audioFormats;

    // filter audio only formats
    std::copy_if(m_videoInfo.formats.begin(), m_videoInfo.formats.end(), std::back_inserter(audioFormats),
        [](const video_info::Format &format) {
        return (!format.acodec.has_value() || format.acodec != "none") && format.vcodec == "none";
    });

    if (audioFormats.empty()) {
        qWarning() << "No audio track found";
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

QString VideoInfoExtractor::artist() const
{
    return QString::fromStdString(m_videoInfo.artist);
}

QString VideoInfoExtractor::album() const
{
    return QString::fromStdString(m_videoInfo.album);
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
