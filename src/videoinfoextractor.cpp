// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "videoinfoextractor.h"

#include <asyncytmusic.h>

#include <QDebug>

VideoInfoExtractor::VideoInfoExtractor(QObject *parent)
    : QObject(parent)
{
    connect(this, &VideoInfoExtractor::videoIdChanged, this, [=] {
        AsyncYTMusic::instance().extractVideoInfo(QString::fromStdString(m_videoId.toStdString()));
    });

    connect(&AsyncYTMusic::instance(), &AsyncYTMusic::extractVideoInfoFinished, this, [=](const video_info::VideoInfo &info) {
        m_videoInfo = info;
        Q_EMIT audioUrlChanged();
        Q_EMIT titleChanged();
    });
}


QUrl VideoInfoExtractor::audioUrl() const
{
    if (m_videoInfo.formats.empty()) {
        return QUrl();
    }

    std::vector<video_info::Format> audioFormats;

    // filter audio only formats
    std::copy_if(m_videoInfo.formats.begin(), m_videoInfo.formats.end(), std::back_inserter(audioFormats),
        [](const video_info::Format &format) {
        return format.acodec != "none" && format.vcodec == "none";
    });

    if (audioFormats.empty()) {
        return QUrl();
    }


    struct {
        bool operator()(const video_info::Format &a, const video_info::Format &b) {
            return a.quality > b.quality;
        }
    } qualitySort;

    std::sort(audioFormats.begin(), audioFormats.end(), qualitySort);

    qDebug() << QString::fromStdString(audioFormats.front().url);

    return QUrl(QString::fromStdString(audioFormats.front().url));
}

QString VideoInfoExtractor::videoId() const
{
    return m_videoId;
}

void VideoInfoExtractor::setVideoId(const QString &videoId)
{
    m_videoId = videoId;
    Q_EMIT videoIdChanged();
}

QString VideoInfoExtractor::title() const
{
    return QString::fromStdString(m_videoInfo.title);
}
