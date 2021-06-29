// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>
#include <QThread>

#include <vector>

#include <ytmusic.h>

Q_DECLARE_METATYPE(std::vector<artist::Artist::Album>);
Q_DECLARE_METATYPE(std::vector<search::SearchResultItem>)
Q_DECLARE_METATYPE(artist::Artist)
Q_DECLARE_METATYPE(album::Album)
Q_DECLARE_METATYPE(song::Song)
Q_DECLARE_METATYPE(playlist::Playlist)
Q_DECLARE_METATYPE(video_info::VideoInfo)
Q_DECLARE_METATYPE(watch::Playlist)
Q_DECLARE_METATYPE(std::optional<QString>)
Q_DECLARE_METATYPE(std::vector<meta::Artist>)
Q_DECLARE_METATYPE(meta::Artist)

///
/// Lazy initialized unique_ptr
///
template <typename T>
class Lazy {
public:
    T *operator->() {
        return get().operator->();
    }

    inline std::unique_ptr<T> &get() {
        if (!m_item) {
            m_item = std::make_unique<T>();
        }
        Q_ASSERT(m_item);
        return m_item;
    }

private:
    std::unique_ptr<T> m_item = nullptr;
};

class AsyncYTMusic : public QObject
{
    friend class YTMusicThread;

    Q_OBJECT

public:
    // public functions need to be thread safe
    void search(const QString &query);
    Q_SIGNAL void searchFinished(std::vector<search::SearchResultItem>);

    void fetchArtist(const QString &channelId);
    Q_SIGNAL void fetchArtistFinished(artist::Artist);

    void fetchAlbum(const QString &browseId);
    Q_SIGNAL void fetchAlbumFinished(album::Album);

    void fetchSong(const QString &videoId);
    Q_SIGNAL void fetchSongFinished(song::Song);

    void fetchPlaylist(const QString &playlistId);
    Q_SIGNAL void fetchPlaylistFinished(playlist::Playlist);

    void fetchArtistAlbums(const QString &channelId, const QString &params);
    Q_SIGNAL void fetchArtistAlbumsFinished(std::vector<artist::Artist::Album>);

    void extractVideoInfo(const QString &videoId);
    Q_SIGNAL void extractVideoInfoFinished(video_info::VideoInfo);

    void fetchWatchPlaylist(const std::optional<QString> &videoId = std::nullopt ,
                            const std::optional<QString> &playlistId = std::nullopt);
    Q_SIGNAL void fetchWatchPlaylistFinished(watch::Playlist);

    Q_SIGNAL void errorOccurred(const QString &error);

protected:
    explicit AsyncYTMusic(QObject *parent = nullptr);

private:
    Q_SIGNAL void startSearch(const QString &query);
    Q_SLOT void internalSearch(const QString &query);

    Q_SIGNAL void startFetchArtist(const QString &channelId);
    Q_SLOT void internalFetchArtist(const QString &channelId);

    Q_SIGNAL void startFetchAlbum(const QString &browseId);
    Q_SLOT void internalFetchAlbum(const QString &browseId);

    Q_SIGNAL void startFetchSong(const QString &videoId);
    Q_SLOT void internalFetchSong(const QString &videoId);

    Q_SIGNAL void startFetchPlaylist(const QString &playlistId);
    Q_SLOT void internalFetchPlaylist(const QString &playlistId);

    Q_SIGNAL void startFetchArtistAlbums(const QString &channelid, const QString &params);
    Q_SLOT void internalFetchArtistAlbums(const QString &channelid, const QString &params);

    Q_SIGNAL void startExtractVideoInfo(const QString &videoId);
    Q_SLOT void internalExtractVideoInfo(const QString &videoId);

    Q_SIGNAL void startFetchWatchPlaylist(const std::optional<QString> &videoId, const std::optional<QString> &playlistId);
    Q_SLOT void internalFetchWatchPlaylist(const std::optional<QString> &videoId, const std::optional<QString> &playlistId);

    // Python interpreter will be initialized from the thread calling the methods
    Lazy<YTMusic> m_ytm;
};

class YTMusicThread : private QThread {
public:
    static YTMusicThread &instance();
    ~YTMusicThread() override;

    AsyncYTMusic *operator->();
    AsyncYTMusic &get();

private:
    YTMusicThread();

    AsyncYTMusic m_ytm;
};
