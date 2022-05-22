// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "asyncytmusic.h"

#include <QThread>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QFutureInterface>

#include <pybind11/embed.h>

#include <iostream>

namespace py = pybind11;

#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <ranges>
#include <type_traits>
#include <memory>

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
QFuture<std::vector<search::SearchResultItem>> AsyncYTMusic::search(const QString &query)
{
    return invokeAndCatchOnThread<std::vector<search::SearchResultItem>>([=, this]() {
        return m_ytm->search(query.toStdString());
    });
}

//
// fetchArtist
//
QFuture<artist::Artist> AsyncYTMusic::fetchArtist(const QString &channelId)
{
    return invokeAndCatchOnThread<artist::Artist>([=, this]() {
        return m_ytm->get_artist(channelId.toStdString());
    });
}

//
// fetchAlbum
//
QFuture<album::Album> AsyncYTMusic::fetchAlbum(const QString &browseId)
{
    return invokeAndCatchOnThread<album::Album>([=, this]() {
        return m_ytm->get_album(browseId.toStdString());
    });
}

//
// fetchSong
//
QFuture<std::optional<song::Song>> AsyncYTMusic::fetchSong(const QString &videoId)
{
    return invokeAndCatchOnThread<std::optional<song::Song>>([=, this]() {
        return m_ytm->get_song(videoId.toStdString());
    });
}

//
// fetchPlaylist
//
QFuture<playlist::Playlist> AsyncYTMusic::fetchPlaylist(const QString &playlistId) {
    return invokeAndCatchOnThread<playlist::Playlist>([=, this]() {
        return m_ytm->get_playlist(playlistId.toStdString());
    });
}

//
// fetchArtistAlbum
//
QFuture<std::vector<artist::Artist::Album>> AsyncYTMusic::fetchArtistAlbums(const QString &channelId, const QString &params)
{
    return invokeAndCatchOnThread<std::vector<artist::Artist::Album>>([=, this]() {
        return m_ytm->get_artist_albums(channelId.toStdString(), params.toStdString());
    });
}

//
// extractVideoInfo
//
QFuture<video_info::VideoInfo> AsyncYTMusic::extractVideoInfo(const QString &videoId)
{
    return invokeAndCatchOnThread<video_info::VideoInfo>([=, this]() {
        return m_ytm->extract_video_info(videoId.toStdString());
    });
}

//
// fetchWatchPlaylist
//
QFuture<watch::Playlist> AsyncYTMusic::fetchWatchPlaylist(const std::optional<QString> &videoId, const std::optional<QString> &playlistId)
{
    return invokeAndCatchOnThread<watch::Playlist>([=, this]() {
        return m_ytm->get_watch_playlist(
            map_optional<std::string>(videoId, &QString::toStdString),
            map_optional<std::string>(playlistId,  &QString::toStdString)
        );
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
