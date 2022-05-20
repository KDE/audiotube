#pragma once

#include <QObject>
#include <QQuickAsyncImageProvider>
#include <QNetworkAccessManager>
#include <QAbstractListModel>

#include "asyncytmusic.h"
#include <lib.rs.h>

class FavouritesModel : public QAbstractListModel {
    Q_OBJECT

    enum Roles {
        VideoId = Qt::UserRole + 1,
        Title,
        Artist,
    };

public:
    FavouritesModel(rust::Vec<Song> songs, QObject *parent = nullptr)
        : QAbstractListModel(parent), m_favouriteSongs(songs) {}

    QHash<int, QByteArray> roleNames() const override {
        return {
            {Roles::VideoId, "videoId"},
            {Roles::Title, "title"},
            {Roles::Artist, "artist"}
        };
    }

    int rowCount(const QModelIndex &parent) const override {
        return parent.isValid() ? 0 : m_favouriteSongs.size();
    }

    QVariant data(const QModelIndex &index, int role) const override {
        switch (role) {
        case Roles::VideoId:
            return QString::fromStdString(std::string(m_favouriteSongs.at(index.row()).video_id));
        case Roles::Title:
            return QString::fromStdString(std::string(m_favouriteSongs.at(index.row()).title));
        case Roles::Artist:
            return QString::fromStdString(std::string(m_favouriteSongs.at(index.row()).artist));
        }

        Q_UNREACHABLE();
    }

private:
    rust::Vec<Song> m_favouriteSongs;
};

class PlaybackHistoryModel : public QAbstractListModel {
    Q_OBJECT

    enum Roles {
        VideoId = Qt::UserRole + 1,
        Title,
        Artist,
        Plays
    };

public:
    PlaybackHistoryModel(rust::Vec<PlayedSong> songs, QObject *parent = nullptr)
        : QAbstractListModel(parent), m_playedSongs(songs) {}

    QHash<int, QByteArray> roleNames() const override {
        return {
            {Roles::VideoId, "videoId"},
            {Roles::Title, "title"},
            {Roles::Artist, "artist"},
            {Roles::Plays, "plays"}
        };
    }

    int rowCount(const QModelIndex &parent) const override {
        return parent.isValid() ? 0 : m_playedSongs.size();
    }

    QVariant data(const QModelIndex &index, int role) const override {
        switch (role) {
        case Roles::VideoId:
            return QString::fromStdString(std::string(m_playedSongs.at(index.row()).video_id));
        case Roles::Title:
            return QString::fromStdString(std::string(m_playedSongs.at(index.row()).title));
        case Roles::Artist:
            return QString::fromStdString(std::string(m_playedSongs.at(index.row()).artist));
        case Roles::Plays:
            return m_playedSongs.at(index.row()).plays;
        }

        Q_UNREACHABLE();
    }

private:
    rust::Vec<PlayedSong> m_playedSongs;
};

class Library : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractListModel *favourites READ favourites NOTIFY favouritesChanged)
    Q_PROPERTY(QStringList searches READ searches NOTIFY searchesChanged)
    Q_PROPERTY(QAbstractListModel *playbackHistory READ playbackHistory NOTIFY playbackHistoryChanged)

public:
    explicit Library(QObject *parent = nullptr);

    static Library &instance();

    FavouritesModel *favourites();
    Q_SIGNAL void favouritesChanged();
    Q_INVOKABLE void addFavourite(const QString &videoId, const QString &title, const QString &artist);
    Q_INVOKABLE void removeFavourite(const QString &videoid);
    Q_INVOKABLE bool isFavourited(const QString &videoId);

    QStringList searches() const;
    Q_SIGNAL void searchesChanged();
    Q_INVOKABLE void addSearch(const QString &text);

    PlaybackHistoryModel *playbackHistory();
    Q_SIGNAL void playbackHistoryChanged();
    Q_INVOKABLE void addPlaybackHistoryItem(const QString &videoId, const QString &title, const QString &artist);

    QNetworkAccessManager &nam();

private:
    QNetworkAccessManager m_networkImageCacher;
    rust::Box<LibraryDatabase> m_database;
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
