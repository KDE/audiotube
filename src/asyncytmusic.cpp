// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "asyncytmusic.h"

#include <QThread>
#include <QDebug>

#include <pybind11/embed.h>

#include <iostream>

namespace py = pybind11;

template <typename R, typename T, typename OP>
std::optional<R> map_optional(const std::optional<T> &optional, OP op) {
    if (optional.has_value()) {
        if constexpr (std::is_member_function_pointer<OP>::value) {
            return (&optional.value()->*op)();
        } else {
            return op(optional.value());
        }
    }

    return std::nullopt;
}

AsyncYTMusic::AsyncYTMusic(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<std::vector<artist::Artist::Album>>();
    qRegisterMetaType<std::vector<search::SearchResultItem>>();
    qRegisterMetaType<artist::Artist>();
    qRegisterMetaType<album::Album>();
    qRegisterMetaType<song::Song>();
    qRegisterMetaType<playlist::Playlist>();
    qRegisterMetaType<video_info::VideoInfo>();
    qRegisterMetaType<watch::Playlist>();
    qRegisterMetaType<std::optional<QString>>();
    qRegisterMetaType<std::vector<meta::Artist>>();
    qRegisterMetaType<meta::Artist>();

    connect(this, &AsyncYTMusic::errorOccurred, this, [](const QString &err) {
        std::cerr << qPrintable(err);
    });
}

//
// search
//
void AsyncYTMusic::search(const QString &query)
{
    invokeAndCatchOnThread([=]() {
        Q_EMIT searchFinished(m_ytm->search(query.toStdString()));
    });
}

//
// fetchArtist
//
void AsyncYTMusic::fetchArtist(const QString &channelId)
{
    invokeAndCatchOnThread([=]() {
        Q_EMIT fetchArtistFinished(m_ytm->get_artist(channelId.toStdString()));
    });
}

//
// fetchAlbum
//
void AsyncYTMusic::fetchAlbum(const QString &browseId)
{
    invokeAndCatchOnThread([=]() {
        Q_EMIT fetchAlbumFinished(m_ytm->get_album(browseId.toStdString()));
    });
}

//
// fetchSong
//
void AsyncYTMusic::fetchSong(const QString &videoId)
{
    invokeAndCatchOnThread([=]() {
        auto maybeSong = m_ytm->get_song(videoId.toStdString());
        if (maybeSong.has_value()) {
            Q_EMIT fetchSongFinished(*maybeSong);
        }
    });
}

//
// fetchPlaylist
//
void AsyncYTMusic::fetchPlaylist(const QString &playlistId) {
    invokeAndCatchOnThread([=]() {
        Q_EMIT fetchPlaylistFinished(m_ytm->get_playlist(playlistId.toStdString()));
    });
}

//
// fetchArtistAlbum
//
void AsyncYTMusic::fetchArtistAlbums(const QString &channelId, const QString &params)
{
    invokeAndCatchOnThread([=]() {
        Q_EMIT fetchArtistAlbumsFinished(m_ytm->get_artist_albums(channelId.toStdString(), params.toStdString()));
    });
}

//
// extractVideoInfo
//
void AsyncYTMusic::extractVideoInfo(const QString &videoId)
{
    invokeAndCatchOnThread([=]() {
        Q_EMIT extractVideoInfoFinished(m_ytm->extract_video_info(videoId.toStdString()));
    });
}

//
// fetchWatchPlaylist
//
void AsyncYTMusic::fetchWatchPlaylist(const std::optional<QString> &videoId, const std::optional<QString> &playlistId)
{
    invokeAndCatchOnThread([=]() {
        Q_EMIT fetchWatchPlaylistFinished(m_ytm->get_watch_playlist(
            map_optional<std::string>(videoId, &QString::toStdString),
            map_optional<std::string>(playlistId,  &QString::toStdString)
        ));
    });
}

void AsyncYTMusic::invokeAndCatchOnThread(const std::function<void()> &fun) {
    QMetaObject::invokeMethod(this, [=]() {
        try {
            fun();
        } catch (const py::error_already_set &err) {
            Q_EMIT errorOccurred(QString::fromLocal8Bit(err.what()));
        }
    });
}

YTMusicThread &YTMusicThread::instance()
{
    static YTMusicThread thread;
    return thread;
}

YTMusicThread::~YTMusicThread()
{
    quit();
    wait();
}

AsyncYTMusic *YTMusicThread::operator->()
{
    return &m_ytm;
}

AsyncYTMusic &YTMusicThread::get()
{
    return m_ytm;
}

YTMusicThread::YTMusicThread()
{
    setObjectName(QStringLiteral("YTMusicAPI"));
    m_ytm.moveToThread(this);
    start();
}
