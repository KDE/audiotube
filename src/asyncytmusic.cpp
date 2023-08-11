// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "asyncytmusic.h"

#include <QThread>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QFutureInterface>
#include <QCoreApplication>
#include <QTimer>

#include <KLocalizedString>

#include <pybind11/embed.h>

#include <iostream>

namespace py = pybind11;

#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <ranges>
#include <type_traits>
#include <memory>

template <typename T, typename OP>
std::optional<std::invoke_result_t<OP, T>> mapOptional(const std::optional<T> &optional, OP op) {
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
        std::cerr << qPrintable(err) << std::endl;
    });

    QTimer::singleShot(0, this, [this]() {
        QCoro::connect(version(), this, [this](auto &&version) {
            if (version != TESTED_YTMUSICAPI_VERSION) {
                Q_EMIT errorOccurred(i18n("Running with untested version of ytmusicapi %1. "
                                          "If you experience errors, please report them to your distribution.", version));
            }
        });
    });
}

//
// search
//
QFuture<std::vector<search::SearchResultItem>> AsyncYTMusic::search(const QString &query)
{
    return invokeAndCatchOnThread([=, this]() {
        return m_ytm->search(query.toStdString());
    });
}

//
// fetchArtist
//
QFuture<artist::Artist> AsyncYTMusic::fetchArtist(const QString &channelId)
{
    return invokeAndCatchOnThread([=, this]() {
        return m_ytm->get_artist(channelId.toStdString());
    });
}

//
// fetchAlbum
//
QFuture<album::Album> AsyncYTMusic::fetchAlbum(const QString &browseId)
{
    return invokeAndCatchOnThread([=, this]() {
        return m_ytm->get_album(browseId.toStdString());
    });
}

//
// fetchSong
//
QFuture<std::optional<song::Song>> AsyncYTMusic::fetchSong(const QString &videoId)
{
    return invokeAndCatchOnThread([=, this]() -> std::optional<song::Song> {
        if (videoId.isEmpty()) {
            return {};
        }

        return m_ytm->get_song(videoId.toStdString());
    });
}

//
// fetchPlaylist
//
QFuture<playlist::Playlist> AsyncYTMusic::fetchPlaylist(const QString &playlistId) {
    return invokeAndCatchOnThread([=, this]() {
        return m_ytm->get_playlist(playlistId.toStdString());
    });
}

//
// fetchArtistAlbum
//
QFuture<std::vector<artist::Artist::Album>> AsyncYTMusic::fetchArtistAlbums(const QString &channelId, const QString &params)
{
    return invokeAndCatchOnThread([=, this]() {
        return m_ytm->get_artist_albums(channelId.toStdString(), params.toStdString());
    });
}

//
// extractVideoInfo
//
QFuture<video_info::VideoInfo> AsyncYTMusic::extractVideoInfo(const QString &videoId)
{
    return invokeAndCatchOnThread([=, this]() {
        return m_ytm->extract_video_info(videoId.toStdString());
    });
}

//
// fetchWatchPlaylist
//
QFuture<watch::Playlist> AsyncYTMusic::fetchWatchPlaylist(const std::optional<QString> &videoId, const std::optional<QString> &playlistId)
{
    return invokeAndCatchOnThread([=, this]() {
        return m_ytm->get_watch_playlist(
            mapOptional(videoId, &QString::toStdString),
            mapOptional(playlistId,  &QString::toStdString)
        );
    });
}

QFuture<Lyrics> AsyncYTMusic::fetchLyrics(const QString &browseId)
{
    return invokeAndCatchOnThread([=, this]() {
        return m_ytm->get_lyrics(
            browseId.toStdString()
        );
    });
}

QFuture<QString> AsyncYTMusic::version()
{
    return invokeAndCatchOnThread([this]() {
        return QString::fromStdString(m_ytm->get_version());
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
    return m_ytm;
}

AsyncYTMusic &YTMusicThread::get()
{
    return *m_ytm;
}

YTMusicThread::YTMusicThread()
    : m_ytm(new AsyncYTMusic())
{
    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, m_ytm, &QObject::deleteLater);
    setObjectName(QStringLiteral("YTMusicAPI"));
    m_ytm->moveToThread(this);
    start();
}

QUrl pickAudioUrl(const std::span<const video_info::Format> formats)
{
    if (formats.empty()) {
        return {};
    }

    std::vector<video_info::Format> audioFormats;

    // filter audio only formats
    std::copy_if(formats.begin(), formats.end(), std::back_inserter(audioFormats),
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
