﻿// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "asyncytmusic.h"

#include <QThread>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QFutureInterface>
#include <QCoreApplication>

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
        std::cerr << qPrintable(err);
    });
}

//
// search
//
QCoro::Task<std::vector<search::SearchResultItem>> AsyncYTMusic::search(const QString &query)
{
    return invokeAndCatchOnThread([=, this]() {
        return m_ytm->search(query.toStdString());
    });
}

//
// fetchArtist
//
QCoro::Task<artist::Artist> AsyncYTMusic::fetchArtist(const QString &channelId)
{
    return invokeAndCatchOnThread([=, this]() {
        return m_ytm->get_artist(channelId.toStdString());
    });
}

//
// fetchAlbum
//
QCoro::Task<album::Album> AsyncYTMusic::fetchAlbum(const QString &browseId)
{
    return invokeAndCatchOnThread([=, this]() {
        return m_ytm->get_album(browseId.toStdString());
    });
}

//
// fetchSong
//
QCoro::Task<std::optional<song::Song>> AsyncYTMusic::fetchSong(const QString &videoId)
{
    return invokeAndCatchOnThread([=, this]() {
        return m_ytm->get_song(videoId.toStdString());
    });
}

//
// fetchPlaylist
//
QCoro::Task<playlist::Playlist> AsyncYTMusic::fetchPlaylist(const QString &playlistId) {
    return invokeAndCatchOnThread([=, this]() {
        return m_ytm->get_playlist(playlistId.toStdString());
    });
}

//
// fetchArtistAlbum
//
QCoro::Task<std::vector<artist::Artist::Album>> AsyncYTMusic::fetchArtistAlbums(const QString &channelId, const QString &params)
{
    return invokeAndCatchOnThread([=, this]() {
        return m_ytm->get_artist_albums(channelId.toStdString(), params.toStdString());
    });
}

//
// extractVideoInfo
//
QCoro::Task<video_info::VideoInfo> AsyncYTMusic::extractVideoInfo(const QString &videoId)
{
    return invokeAndCatchOnThread([=, this]() {
        return m_ytm->extract_video_info(videoId.toStdString());
    });
}

//
// fetchWatchPlaylist
//
QCoro::Task<watch::Playlist> AsyncYTMusic::fetchWatchPlaylist(const std::optional<QString> &videoId, const std::optional<QString> &playlistId)
{
    return invokeAndCatchOnThread([=, this]() {
        return m_ytm->get_watch_playlist(
            mapOptional(videoId, &QString::toStdString),
            mapOptional(playlistId,  &QString::toStdString)
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
