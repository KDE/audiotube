// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>
#include <QQuickAsyncImageProvider>
#include <QNetworkAccessManager>
#include <QAbstractListModel>
#include <QSortFilterProxyModel>

#include <memory>

#include "asyncytmusic.h"
#include "asyncdatabase.h"

#include "autolistmodel.h"

using namespace desert;

class FavouriteWatcher;
class WasPlayedWatcher;

inline std::vector<meta::Artist> metaArtist(const QString &artist) {
    return { meta::Artist { artist.toStdString(), {} } };
}

struct Song {
    DESERT_OBJECT

    int operator<=>(const Song &other) const = default;

    using ColumnTypes = std::tuple<QString, QString, QString, QString>;

    static Song fromSql(ColumnTypes tuple) {
        auto [videoId, title, artist, album] = tuple;
        return Song {videoId, title, artist, album};
    }

    Attribute<u"videoId", QString> videoId;
    Attribute<u"title", QString> title;
    Attribute<u"artists", QString> artists;
    Attribute<u"album", QString> album;
};

class Library;

class FavouritesModel : public AutoListModel<Song> {
    Q_OBJECT

public:
    FavouritesModel(Library *library);
};

struct PlayedSong {
    DESERT_OBJECT

    int operator<=>(const PlayedSong &other) const = default;

    using ColumnTypes = std::tuple<QString, int, QString, QString, QString>;

    static PlayedSong fromSql(ColumnTypes tuple) {
        auto [videoId, plays, title, artist, album] = tuple;
        return PlayedSong {videoId, title, artist, album, plays};
    }

    Attribute<u"videoId", QString> videoId;
    Attribute<u"title", QString> title;
    Attribute<u"artists", QString> artists;
    Attribute<u"album", QString> album;
    Attribute<u"album", int> plays;
};

class PlaybackHistoryModel : public AutoListModel<PlayedSong> {
    Q_OBJECT

public:
    enum Type {
        MostPlayed,
        History
    };
    PlaybackHistoryModel(Library *library, Type type);
};

class SearchQuery {
    DESERT_OBJECT

    using ColumnTypes = std::tuple<QString>;

    static SearchQuery fromSql(ColumnTypes tuple) {
        auto [query] = tuple;
        return { query };
    }

    int operator<=>(const SearchQuery &other) const = default;

    Attribute<u"query", QString> query;
};

class SearchHistoryModel : public AutoListModel<SearchQuery> {
    Q_OBJECT

public:
    SearchHistoryModel(Library *library);

private:
    std::vector<SearchQuery> m_history;
};

class Library : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractListModel *favourites READ favourites CONSTANT)
    Q_PROPERTY(QAbstractListModel *searches READ searches CONSTANT)
    Q_PROPERTY(QAbstractListModel *playbackHistory READ playbackHistory CONSTANT)
    Q_PROPERTY(QAbstractListModel *mostPlayed READ mostPlayed CONSTANT)

public:
    explicit Library(QObject *parent = nullptr);
    ~Library();

    static Library &instance();

    FavouritesModel *favourites();
    Q_SIGNAL void favouritesChanged();
    Q_INVOKABLE void addFavourite(const QString &videoId, const QString &title, const QString &artist, const QString &album);
    Q_INVOKABLE void removeFavourite(const QString &videoId);
    Q_INVOKABLE FavouriteWatcher *favouriteWatcher(const QString &videoId);

    SearchHistoryModel *searches();
    Q_SIGNAL void searchesChanged();
    Q_INVOKABLE void addSearch(const QString &text);
    Q_INVOKABLE void removeSearch(const QString &text);

    PlaybackHistoryModel *playbackHistory();
    Q_SIGNAL void playbackHistoryChanged();
    Q_INVOKABLE void addPlaybackHistoryItem(const QString &videoId, const QString &title, const QString &artist, const QString &album);
    Q_INVOKABLE void removePlaybackHistoryItem(const QString &videoId);
    Q_INVOKABLE WasPlayedWatcher *wasPlayedWatcher(const QString &videoId);

    PlaybackHistoryModel *mostPlayed();

    QNetworkAccessManager &nam();
    ThreadedDatabase &database() {
        return *m_database;
    }

private:
    QFuture<void> addSong(const QString &videoId, const QString &title, const QString &artist, const QString &album);

    QNetworkAccessManager m_networkImageCacher;
    std::unique_ptr<ThreadedDatabase> m_database;
};


class ThumbnailSource : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString videoId READ videoId WRITE setVideoId NOTIFY videoIdChanged)
    Q_PROPERTY(QUrl cachedPath READ cachedPath NOTIFY cachedPathChanged)

    QString videoId() const {
        return m_videoId;
    }
    void setVideoId(const QString &id);
    Q_SIGNAL void videoIdChanged();

    QUrl cachedPath() const {
        return m_cachedPath;
    }
    void setCachedPath(const QUrl &path) {
        m_cachedPath = path;
        cachedPathChanged();
    }
    Q_SIGNAL void cachedPathChanged();

private:
    QString m_videoId;
    QUrl m_cachedPath;
};

class FavouriteWatcher : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool isFavourite READ isFavourite NOTIFY isFavouriteChanged)

public:
    FavouriteWatcher(Library *library, const QString &videoId);

    bool isFavourite() const;
    Q_SIGNAL void isFavouriteChanged();

    Q_SIGNAL void videoIdChanged();

private:
    QString m_videoId;
    Library *m_library;
    bool m_isFavourite = false;
};

class WasPlayedWatcher : public QObject {
    Q_OBJECT
    
    Q_PROPERTY(bool wasPlayed READ wasPlayed NOTIFY wasPlayedChanged)
    
public:
    WasPlayedWatcher(Library *Library, const QString &VideoId);
    
    bool wasPlayed() const;
    
    Q_SIGNAL void wasPlayedChanged();

private:
    bool m_wasPlayed = false;
    QString m_videoId;
    Library *m_library;
    Q_SLOT void update(std::optional<SingleValue<bool>> result);
    Q_SLOT void query();
};
