// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "library.h"

#include <QStandardPaths>
#include <QDir>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStringBuilder>

#include "asyncdatabase.h"

namespace ranges = std::ranges;

Library::Library(QObject *parent)
    : QObject{parent}
    , m_database(ThreadedDatabase::establishConnection([]() -> DatabaseConfiguration {
        const auto databaseDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        // Make sure the database directory exists
        QDir(databaseDirectory).mkpath(QStringLiteral("."));

        DatabaseConfiguration config;
        config.setDatabaseName(databaseDirectory % QDir::separator() % "library.sqlite");
        config.setType(DATABASE_TYPE_SQLITE);
        return config;
    }()))
{
    m_database->runMigrations(":/migrations/");
}

Library::~Library() = default;

Library &Library::instance()
{
    static Library inst;
    return inst;
}

FavouritesModel *Library::favourites()
{
    auto future = m_database->getResults<Song>("select * from favourites natural join songs");
    return new FavouritesModel(std::move(future), this);
}

void Library::addFavourite(const QString &videoId, const QString &title)
{
    connectFuture(addSong(videoId, title), this, [=, this] {
        connectFuture(m_database->execute("insert or ignore into favourites (video_id) values (?)", videoId), this, &Library::favouritesChanged);
    });
}

void Library::removeFavourite(const QString &videoId)
{
    connectFuture(m_database->execute("delete from favourites where video_id = ?", videoId), this, &Library::favouritesChanged);
}

FavouriteWatcher *Library::favouriteWatcher(const QString &videoId)
{
    if (videoId.isEmpty()) {
        return nullptr;
    }
    return new FavouriteWatcher(this, videoId);
}

SearchHistoryModel *Library::searches()
{
    return new SearchHistoryModel(m_database->getResults<SingleValue<QString>>("select (search_query) from searches order by search_id desc"), this);
}

void Library::addSearch(const QString &text)
{
    connectFuture(m_database->execute("insert into searches (search_query) values (?)", text), this, &Library::searchesChanged);
}

PlaybackHistoryModel *Library::playbackHistory()
{
    auto future = m_database->getResults<PlayedSong>("select * from played_songs natural join songs");
    return new PlaybackHistoryModel(std::move(future), this);
}

void Library::addPlaybackHistoryItem(const QString &videoId, const QString &title)
{
    connectFuture(addSong(videoId, title), this, [=, this] {
        connectFuture(m_database->execute("insert or ignore into played_songs (video_id, plays) values (?, ?)", videoId, 0), this, [=, this] {
            connectFuture(m_database->execute("update played_songs set plays = plays + 1 where video_id = ? ", videoId), this, &Library::playbackHistoryChanged);
        });
    });
}

PlaybackHistoryModel *Library::mostPlayed()
{
    auto future = m_database->getResults<PlayedSong>("select * from played_songs natural join songs order by plays desc limit 10");
    return new PlaybackHistoryModel(std::move(future), this);
}

QNetworkAccessManager &Library::nam()
{
    return m_networkImageCacher;
}

QFuture<void> Library::addSong(const QString &videoId, const QString &title)
{
    return m_database->execute("insert or ignore into songs (video_id, title, artist, album) values (?, ?, null, null)", videoId, title);
}

void ThumbnailSource::setVideoId(const QString &id) {
    if (m_videoId == id) {
        return;
    }

    m_videoId = id;
    Q_EMIT videoIdChanged();

    const QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) % QDir::separator() % "thumbnails";
    QDir(cacheDir).mkpath(QStringLiteral("."));
    const QString cacheLocation = cacheDir % QDir::separator() % id % ".webp";

    if (QFile::exists(cacheLocation)) {
        setCachedPath(QUrl::fromLocalFile(cacheLocation));
        return;
    }

    auto future = YTMusicThread::instance()->extractVideoInfo(id);
    connectFuture(future, this, [this, id, cacheLocation](video_info::VideoInfo &&info) {
        auto *reply = Library::instance().nam().get(QNetworkRequest(QUrl(QString::fromStdString(info.thumbnail))));
        connect(reply, &QNetworkReply::finished, this, [id, reply, this, cacheLocation]() {
            QFile file(cacheLocation);
            file.open(QFile::WriteOnly);
            file.write(reply->readAll());
            setCachedPath(QUrl::fromLocalFile(cacheLocation));
        });
    });
}

PlaybackHistoryModel::PlaybackHistoryModel(QFuture<std::vector<PlayedSong> > &&songs, QObject *parent)
    : QAbstractListModel(parent)
{
    connectFuture(songs, this, [this](const auto songs) {
        beginResetModel();
        m_playedSongs = songs;
        endResetModel();
    });
}

QHash<int, QByteArray> PlaybackHistoryModel::roleNames() const {
    return {
        {Roles::VideoId, "videoId"},
        {Roles::Title, "title"},
        {Roles::Artist, "artists"},
        {Roles::Plays, "plays"}
    };
}

int PlaybackHistoryModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_playedSongs.size();
}

QVariant PlaybackHistoryModel::data(const QModelIndex &index, int role) const {
    switch (role) {
    case Roles::VideoId:
        return m_playedSongs.at(index.row()).videoId;
    case Roles::Title:
        return m_playedSongs.at(index.row()).title;
    case Roles::Artist:
        return m_playedSongs.at(index.row()).artist;
    case Roles::Plays:
        return m_playedSongs.at(index.row()).plays;
    }

    Q_UNREACHABLE();
}

FavouritesModel::FavouritesModel(QFuture<std::vector<Song>> &&songs, QObject *parent)
    : QAbstractListModel(parent)
{
    connectFuture(songs, this, [this](const auto songs) {
        beginResetModel();
        m_favouriteSongs = songs;
        endResetModel();
    });
}

QHash<int, QByteArray> FavouritesModel::roleNames() const {
    return {
        {Roles::VideoId, "videoId"},
        {Roles::Title, "title"},
        {Roles::Artist, "artists"}
    };
}

int FavouritesModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_favouriteSongs.size();
}

QVariant FavouritesModel::data(const QModelIndex &index, int role) const {
    switch (role) {
    case Roles::VideoId:
        return m_favouriteSongs.at(index.row()).videoId;
    case Roles::Title:
        return m_favouriteSongs.at(index.row()).title;
    case Roles::Artist:
        return m_favouriteSongs.at(index.row()).artist;
    }

    Q_UNREACHABLE();
}

FavouriteWatcher::FavouriteWatcher(Library *library, const QString &videoId)
    : QObject(library), m_videoId(videoId), m_library(library)
{
    auto update = [this] {
        connectFuture(m_library->database().getResult<SingleValue<int>>("select count(*) from favourites where video_id = ?", m_videoId), this, [this](auto count) {
            if (count) {
                m_isFavourite = count->value > 0;
                Q_EMIT isFavouriteChanged();
            }
        });
    };
    update();
    connect(library, &Library::favouritesChanged, this, update);
}

bool FavouriteWatcher::isFavourite() const {
    return m_isFavourite;
}

SearchHistoryModel::SearchHistoryModel(QFuture<std::vector<SingleValue<QString>>> &&historyFuture, QObject *parent)
    : QAbstractListModel(parent)
{
    connectFuture(historyFuture, this, [this](const auto history) {
        beginResetModel();
        m_history = history;
        endResetModel();
    });
}

int SearchHistoryModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_history.size();
}

QVariant SearchHistoryModel::data(const QModelIndex &index, int role) const {
    switch (role) {
    case Qt::DisplayRole:
        return m_history[index.row()].value;
    }

    Q_UNREACHABLE();
}
