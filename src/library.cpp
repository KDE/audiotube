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
    auto future = m_database->getResults<Song>("select * from favourites natural join songs order by favourites.rowid desc");
    return new FavouritesModel(std::move(future), this);
}

void Library::addFavourite(const QString &videoId, const QString &title, const QString &artist, const QString &album)
{
    connectFuture(addSong(videoId, title, artist, album), this, [=, this] {
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
    return new SearchHistoryModel(m_database->getResults<SingleValue<QString>>("select distinct (search_query) from searches order by search_id desc"), this);
}

void Library::addSearch(const QString &text)
{
    connectFuture(m_database->execute("insert into searches (search_query) values (?)", text), this, &Library::searchesChanged);
}

void Library::removeSearch(const QString &text)
{
    connectFuture(m_database->execute("delete from searches where search_query = ?", text), this, &Library::searchesChanged);
}

PlaybackHistoryModel *Library::playbackHistory()
{
    auto future = m_database->getResults<PlayedSong>("select * from played_songs natural join songs");
    return new PlaybackHistoryModel(std::move(future), this);
}

void Library::addPlaybackHistoryItem(const QString &videoId, const QString &title, const QString &artist, const QString &album)
{
    connectFuture(addSong(videoId, title, artist, album), this, [=, this] {
        connectFuture(m_database->execute("insert or ignore into played_songs (video_id, plays) values (?, ?)", videoId, 0), this, [=, this] {
            connectFuture(m_database->execute("update played_songs set plays = plays + 1 where video_id = ? ", videoId), this, &Library::playbackHistoryChanged);
        });
    });
}

void Library::removePlaybackHistoryItem(const QString &videoId)
{
    connectFuture(m_database->execute("delete from played_songs where video_id = ?", videoId), this, &Library::playbackHistoryChanged);
}

WasPlayedWatcher *Library::wasPlayedWatcher(const QString& videoId)
{
    if(videoId.isEmpty()){
        return nullptr;
    }
    return new WasPlayedWatcher(this, videoId);
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

QFuture<void> Library::addSong(const QString &videoId, const QString &title, const QString &artist, const QString &album)
{
    // replace is used here to update songs from times when we didn't store artist and album
    return m_database->execute("insert or replace into songs (video_id, title, artist, album) values (?, ?, ?, ?)", videoId, title, artist, album);
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

    auto *reply = Library::instance().nam().get(QNetworkRequest(QUrl("https://i.ytimg.com/vi_webp/" % m_videoId % "/maxresdefault.webp")));

    auto storeResult = [this, cacheLocation](QNetworkReply *reply) {
        QFile file(cacheLocation);
        file.open(QFile::WriteOnly);
        file.write(reply->readAll());
        setCachedPath(QUrl::fromLocalFile(cacheLocation));

        reply->deleteLater();
    };

    connect(reply, &QNetworkReply::errorOccurred, this, [this, storeResult](auto error) {
        if (error == QNetworkReply::NetworkError::ContentNotFoundError) {
            qDebug() << "Naive thumbnail resolution failed, falling back to yt-dlp (slower)";

            connectFuture(YTMusicThread::instance()->extractVideoInfo(m_videoId), this, [this, storeResult](auto info) {
                auto *reply = Library::instance().nam().get(QNetworkRequest(QUrl(QString::fromStdString(info.thumbnail))));
                connect(reply, &QNetworkReply::finished, this, [reply, storeResult]() {
                    storeResult(reply);
                });
            });
        }
    });

    connect(reply, &QNetworkReply::finished, this, [reply, storeResult]() {
        storeResult(reply);
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
        {Roles::Artists, "artists"},
        {Roles::ArtistsDisplayString, "artistsDisplayString"},
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
    case Roles::Artists:
        return QVariant::fromValue(std::vector<meta::Artist> {
            {
                m_playedSongs.at(index.row()).artist.toStdString(),
                {}
            }
        });
    case Roles::ArtistsDisplayString:
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
        {Roles::Artists, "artists"},
        {Roles::ArtistsDisplayString, "artistsDisplayString"}
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
    case Roles::ArtistsDisplayString:
        return m_favouriteSongs.at(index.row()).artist;
    case Roles::Artists:
        return QVariant::fromValue(std::vector<meta::Artist> {
            {
                m_favouriteSongs.at(index.row()).artist.toStdString(),
                {}
            }
        });
    }

    Q_UNREACHABLE();
}

FavouriteWatcher::FavouriteWatcher(Library *library, const QString &videoId)
    : QObject(library), m_videoId(videoId), m_library(library)
{
    auto update = [this] {
        connectFuture(m_library->database().getResult<SingleValue<bool>>("select count(*) > 0 from favourites where video_id = ?", m_videoId), this, [this](auto count) {
            if (count) {
                m_isFavourite = count->value;
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

bool SearchHistoryModel::removeRows(int row, int count, const QModelIndex &parent) {
    if((unsigned) row+count >= m_history.size()) {
        return false;
    }
    beginRemoveRows(parent, row, row+count-1);
    m_history.erase(m_history.begin() + row, m_history.begin() + (row + count));
    endRemoveRows();
    return true;
}

int SearchHistoryModel::getRow(const QString& search) const {
    for(size_t i(0); i<m_history.size(); ++i) {
        if(m_history[i].value == search) {
            return i;
        }
    }
    return -1;
}


WasPlayedWatcher::WasPlayedWatcher(Library* library, const QString& videoId)
    : QObject(library), m_videoId(videoId), m_library(library)
{
    connect(m_library, &Library::playbackHistoryChanged, this, &WasPlayedWatcher::query);
    query();
}

void WasPlayedWatcher::query()
{
    connectFuture(m_library->database().getResult<SingleValue<bool>>("select count(*) > 0 from played_songs where video_id = ?", m_videoId), this, &WasPlayedWatcher::update);
}


bool WasPlayedWatcher::wasPlayed() const
{
    return m_wasPlayed;
}


void WasPlayedWatcher::update(std::optional<SingleValue<bool>> result)
{
    if(result.has_value())
    {
        m_wasPlayed = result->value;
        Q_EMIT wasPlayedChanged();
    }
}
