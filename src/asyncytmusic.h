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

class AsyncYTMusic : public QObject
{
    Q_OBJECT

public:
    explicit AsyncYTMusic(QObject *parent = nullptr);

    ///
    /// Singleton instance of AsyncYtMusic running on it's own thread.
    ///
    /// It is necessary t call stopInstance before the application exits,
    /// so the thread can properly finish.
    ///
    static AsyncYTMusic &instance();
    static void stopInstance();

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

    YTMusic m_ytdl;
};
