// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "asyncytmusic.h"

#include <QThread>
#include <exception>
#include <QDebug>

#include <pybind11/embed.h>

namespace py = pybind11;

static QThread *const ytmthread = []() -> QThread * {
    auto *thread = new QThread();
    thread->setObjectName(QStringLiteral("YTMusicAPI"));
    return thread;
}();

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

    connect(this, &AsyncYTMusic::startSearch, this, &AsyncYTMusic::internalSearch);
    connect(this, &AsyncYTMusic::startFetchArtist, this, &AsyncYTMusic::internalFetchArtist);
    connect(this, &AsyncYTMusic::startFetchAlbum, this, &AsyncYTMusic::internalFetchAlbum);
    connect(this, &AsyncYTMusic::startFetchSong, this, &AsyncYTMusic::internalFetchSong);
    connect(this, &AsyncYTMusic::startFetchPlaylist, this, &AsyncYTMusic::internalFetchPlaylist);
    connect(this, &AsyncYTMusic::startFetchArtistAlbums, this, &AsyncYTMusic::internalFetchArtistAlbums);
    connect(this, &AsyncYTMusic::startExtractVideoInfo, this, &AsyncYTMusic::internalExtractVideoInfo);
    connect(this, &AsyncYTMusic::startFetchWatchPlaylist, this, &AsyncYTMusic::internalFetchWatchPlaylist);
}

AsyncYTMusic &AsyncYTMusic::instance()
{
    static AsyncYTMusic &inst = []() -> AsyncYTMusic& {
        static AsyncYTMusic ytm;
        ytm.moveToThread(ytmthread);
        ytmthread->start();

        return ytm;
    }();

    return inst;
}

void AsyncYTMusic::stopInstance()
{
    ytmthread->quit();
    ytmthread->wait();
}

//
// search
//
void AsyncYTMusic::search(const QString &query)
{
    Q_EMIT startSearch(query);
}

void AsyncYTMusic::internalSearch(const QString &query)
{
    Q_EMIT searchFinished(m_ytdl.search(query.toStdString()));
}

//
// fetchArtist
//
void AsyncYTMusic::fetchArtist(const QString &channelId)
{
    Q_EMIT startFetchArtist(channelId);
}

void AsyncYTMusic::internalFetchArtist(const QString &channelId)
{
    Q_EMIT fetchArtistFinished(m_ytdl.get_artist(channelId.toStdString()));
}

//
// fetchAlbum
//
void AsyncYTMusic::fetchAlbum(const QString &browseId)
{
    Q_EMIT startFetchAlbum(browseId);
}

void AsyncYTMusic::internalFetchAlbum(const QString &browseId)
{
    Q_EMIT fetchAlbumFinished(m_ytdl.get_album(browseId.toStdString()));
}

//
// fetchSong
//
void AsyncYTMusic::fetchSong(const QString &videoId)
{
    Q_EMIT startFetchSong(videoId);
}

void AsyncYTMusic::internalFetchSong(const QString &videoId)
{
    Q_EMIT fetchSongFinished(m_ytdl.get_song(videoId.toStdString()));
}

//
// fetchPlaylist
//
void AsyncYTMusic::fetchPlaylist(const QString &playlistId) {
    Q_EMIT startFetchPlaylist(playlistId);
}

void AsyncYTMusic::internalFetchPlaylist(const QString &playlistId)
{
    Q_EMIT fetchPlaylistFinished(m_ytdl.get_playlist(playlistId.toStdString()));
}

//
// fetchArtistAlbum
//
void AsyncYTMusic::fetchArtistAlbums(const QString &channelId, const QString &params)
{
    Q_EMIT startFetchArtistAlbums(channelId, params);
}

void AsyncYTMusic::internalFetchArtistAlbums(const QString &channelid, const QString &params)
{
    Q_EMIT fetchArtistAlbumsFinished(m_ytdl.get_artist_albums(channelid.toStdString(), params.toStdString()));
}

//
// extractVideoInfo
//
void AsyncYTMusic::extractVideoInfo(const QString &videoId) {
    Q_EMIT startExtractVideoInfo(videoId);
}

void AsyncYTMusic::internalExtractVideoInfo(const QString &videoId) {
    Q_EMIT extractVideoInfoFinished(m_ytdl.extract_video_info(videoId.toStdString()));
}

//
// fetchWatchPlaylist
//
void AsyncYTMusic::fetchWatchPlaylist(const std::optional<QString> &videoId, const std::optional<QString> &playlistId)
{
    Q_EMIT startFetchWatchPlaylist(videoId, playlistId);
}

void AsyncYTMusic::internalFetchWatchPlaylist(const std::optional<QString> &videoId, const std::optional<QString> &playlistId)
{
    Q_EMIT fetchWatchPlaylistFinished(m_ytdl.get_watch_playlist(
    [&]() -> std::optional<std::string> {
        if (!videoId.has_value()) {
            return std::nullopt;
        }

        return videoId->toStdString();
    }(),
    [&]() -> std::optional<std::string> {
        if (!playlistId.has_value()) {
            return std::nullopt;
        }

        return playlistId->toStdString();
    }()));
}
