// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "asyncytmusic.h"

#include <QThread>
#include <exception>
#include <QDebug>

#include <pybind11/embed.h>

namespace py = pybind11;


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

    connect(this, &AsyncYTMusic::startSearch, this, &AsyncYTMusic::internalSearch);
    connect(this, &AsyncYTMusic::startFetchArtist, this, &AsyncYTMusic::internalFetchArtist);
    connect(this, &AsyncYTMusic::startFetchAlbum, this, &AsyncYTMusic::internalFetchAlbum);
    connect(this, &AsyncYTMusic::startFetchSong, this, &AsyncYTMusic::internalFetchSong);
    connect(this, &AsyncYTMusic::startFetchPlaylist, this, &AsyncYTMusic::internalFetchPlaylist);
    connect(this, &AsyncYTMusic::startFetchArtistAlbums, this, &AsyncYTMusic::internalFetchArtistAlbums);
    connect(this, &AsyncYTMusic::startExtractVideoInfo, this, &AsyncYTMusic::internalExtractVideoInfo);
    connect(this, &AsyncYTMusic::startFetchWatchPlaylist, this, &AsyncYTMusic::internalFetchWatchPlaylist);

    connect(this, &AsyncYTMusic::errorOccurred, this, [](const QString &err) {
        qDebug() << err;
    });
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
    try {
        Q_EMIT searchFinished(m_ytm->search(query.toStdString()));
    } catch (const py::error_already_set &err) {
        Q_EMIT errorOccurred(QString::fromLocal8Bit(err.what()));
    }
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
    try {
        Q_EMIT fetchArtistFinished(m_ytm->get_artist(channelId.toStdString()));
    } catch (const py::error_already_set &err) {
        Q_EMIT errorOccurred(QString::fromLocal8Bit(err.what()));
    }
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
    try {
        Q_EMIT fetchAlbumFinished(m_ytm->get_album(browseId.toStdString()));
    } catch (const py::error_already_set &err) {
        Q_EMIT errorOccurred(QString::fromLocal8Bit(err.what()));
    }
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
    try {
        Q_EMIT fetchSongFinished(m_ytm->get_song(videoId.toStdString()));
    } catch (const py::error_already_set &err) {
        Q_EMIT errorOccurred(QString::fromLocal8Bit(err.what()));
    }
}

//
// fetchPlaylist
//
void AsyncYTMusic::fetchPlaylist(const QString &playlistId) {
    Q_EMIT startFetchPlaylist(playlistId);
}

void AsyncYTMusic::internalFetchPlaylist(const QString &playlistId)
{
    try {
        Q_EMIT fetchPlaylistFinished(m_ytm->get_playlist(playlistId.toStdString()));
    } catch (const py::error_already_set &err) {
        Q_EMIT errorOccurred(QString::fromLocal8Bit(err.what()));
    }
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
    try {
        Q_EMIT fetchArtistAlbumsFinished(m_ytm->get_artist_albums(channelid.toStdString(), params.toStdString()));
    } catch (const py::error_already_set &err) {
        Q_EMIT errorOccurred(QString::fromLocal8Bit(err.what()));
    }
}

//
// extractVideoInfo
//
void AsyncYTMusic::extractVideoInfo(const QString &videoId)
{
    Q_EMIT startExtractVideoInfo(videoId);
}

void AsyncYTMusic::internalExtractVideoInfo(const QString &videoId)
{
    try {
        Q_EMIT extractVideoInfoFinished(m_ytm->extract_video_info(videoId.toStdString()));
    } catch (const py::error_already_set &err) {
        Q_EMIT errorOccurred(QString::fromLocal8Bit(err.what()));
    }
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
    try {
        Q_EMIT fetchWatchPlaylistFinished(m_ytm->get_watch_playlist(
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
    } catch (const py::error_already_set &err) {
        Q_EMIT errorOccurred(QString::fromLocal8Bit(err.what()));
    }
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
