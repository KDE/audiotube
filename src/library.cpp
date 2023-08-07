// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "library.h"

#include <QStandardPaths>
#include <QDir>
#include <QStringBuilder>
#include <QGuiApplication>

#include <ThreadedDatabase>

namespace ranges = std::ranges;

Library::Library(QObject *parent)
    : QObject{parent}
    , m_database(ThreadedDatabase::establishConnection([]() -> DatabaseConfiguration {
        const auto databaseDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        // Make sure the database directory exists
        QDir(databaseDirectory).mkpath(QStringLiteral("."));

        DatabaseConfiguration config;
        config.setDatabaseName(databaseDirectory % QDir::separator() % "library.sqlite");
        config.setType(DatabaseType::SQLite);
        return config;
    }()))
{
    m_database->runMigrations(":/migrations/");
    m_searches = new SearchHistoryModel(this);

    refreshFavourites();
    refreshPlaybackHistory();
}

Library::~Library() = default;

Library &Library::instance()
{
    static Library inst;
    return inst;
}

FavouritesModel *Library::favourites()
{
    return m_favourites;
}

void Library::addFavourite(const QString &videoId, const QString &title, const QString &artist, const QString &album)
{
    QCoro::connect(addSong(videoId, title, artist, album), this, [=, this] {
        QCoro::connect(m_database->execute("insert or ignore into favourites (video_id) values (?)", videoId),
                       this, &Library::refreshFavourites);
    });
}

void Library::removeFavourite(const QString &videoId)
{
    QCoro::connect(m_database->execute("delete from favourites where video_id = ?", videoId),
                   this, &Library::refreshFavourites);
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
    return m_searches;
}

void Library::addSearch(const QString &text)
{
    m_searches->addSearch(text);
    QCoro::connect(m_database->execute("insert into searches (search_query) values (?)", text), this, &Library::searchesChanged);
}

void Library::removeSearch(const QString &text) {
    m_searches->removeSearch(text);
    QCoro::connect(m_database->execute("delete from searches where search_query = ?", text), this, &Library::searchesChanged);
}

const QString& Library::temporarySearch()
{
    return m_searches->temporarySearch();
}

void Library::setTemporarySearch(const QString& text)
{
    m_searches->setTemporarySearch(text);
    Q_EMIT temporarySearchChanged();
}


PlaybackHistoryModel *Library::playbackHistory()
{
    return m_playbackHistory;
}

void Library::refreshPlaybackHistory()
{
    // playbackHistory
    auto future = m_database->getResults<PlayedSong>(
        "select * from played_songs natural join songs");
    m_playbackHistory = new PlaybackHistoryModel(std::move(future), this);

    // mostPlayed
    auto future2 = m_database->getResults<PlayedSong>(
        "select * from played_songs natural join songs order by plays desc limit 10");
    m_mostPlayed = new PlaybackHistoryModel(std::move(future2), this);
    Q_EMIT playbackHistoryChanged();
}

void Library::refreshFavourites()
{
    auto future = m_database->getResults<Song>(
        "select * from favourites natural join songs order by favourites.rowid desc");
    m_favourites = new FavouritesModel(std::move(future), this);
    Q_EMIT favouritesChanged();
}

void Library::addPlaybackHistoryItem(const QString &videoId, const QString &title, const QString &artist, const QString &album)
{
    QCoro::connect(addSong(videoId, title, artist, album), this, [=, this] {
        QCoro::connect(m_database->execute("insert or ignore into played_songs (video_id, plays) values (?, ?)", videoId, 0), this, [=, this] {
            QCoro::connect(m_database->execute("update played_songs set plays = plays + 1 where video_id = ? ", videoId),
                           this, &Library::refreshPlaybackHistory);
        });
    });
}
void Library::removePlaybackHistoryItem(const QString &videoId)
{
    QCoro::connect(m_database->execute("delete from played_songs where video_id = ?", videoId),
                   this, &Library::refreshPlaybackHistory);
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
    return m_mostPlayed;
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

PlaybackHistoryModel::PlaybackHistoryModel(QFuture<std::vector<PlayedSong>> &&songs, QObject *parent)
    : QAbstractListModel(parent)
{
    QCoro::connect(std::move(songs), this, [this](const auto songs) {
        beginResetModel();
        m_playedSongs = songs;
        endResetModel();
    });
}

PlaybackHistoryModel::PlaybackHistoryModel(QObject *parent)
    : QAbstractListModel(parent)
{
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

std::vector<PlayedSong> PlaybackHistoryModel::getPlayedSong() const
{
    return m_playedSongs;
}


FavouritesModel::FavouritesModel(QFuture<std::vector<Song>> &&songs, QObject *parent)
    : QAbstractListModel(parent)
{
    QCoro::connect(std::move(songs), this, [this](const auto songs) {
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

std::vector<Song> FavouritesModel::getFavouriteSongs() const {
    return m_favouriteSongs;
}

FavouriteWatcher::FavouriteWatcher(Library *library, const QString &videoId)
    : QObject(library), m_videoId(videoId), m_library(library)
{
    auto update = [this] {
        QCoro::connect(m_library->database().getResult<SingleValue<bool>>("select count(*) > 0 from favourites where video_id = ?", m_videoId), this, [this](auto count) {
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

SearchHistoryModel::SearchHistoryModel(Library *library)
    : QAbstractListModel(library)
{
    auto historyFuture = library->database()
                            .getResults<SingleValue<QString>>("select distinct (search_query) from searches order by search_id desc limit 20");

    connect(this, &SearchHistoryModel::filterChanged, this, [library, this]() {
        auto future = library->database()
            .getResults<SingleValue<QString>>("select distinct (search_query) from searches "
                                              "where search_query like '%" % m_filter % "%'"
                                              "order by search_id desc limit 20");

        QCoro::connect(std::move(future), this, [this](auto history) {
            beginResetModel();
            m_history = history;
            endResetModel();
        });
    });

    QCoro::connect(std::move(historyFuture), this, [this](const auto history) {
        beginResetModel();
        m_history = history;
        endResetModel();
    });
}

int SearchHistoryModel::rowCount(const QModelIndex &parent) const {
    if(parent.isValid()) {
        return 0;
    }
    else if (temporarySearch().isEmpty()) {
        return m_history.size();
    }
    else {
        return m_history.size() + 1;
    }
}

void SearchHistoryModel::removeSearch(const QString &search) {
    int row = getRow(search);
    beginRemoveRows({}, row, row);
    m_history.erase(m_history.begin() + row);
    endRemoveRows();
}

size_t SearchHistoryModel::getRow(const QString &search) const {
    auto itr = find_if(m_history.begin(), m_history.end(), [&](const auto &checkedValue) {
        return checkedValue.value == search;
    });
    size_t i = std::distance(m_history.begin(), itr);
    if(m_temporarySearch != "") {
        ++i;
    }
    Q_ASSERT(i < m_history.size());
    return i;
}

QVariant SearchHistoryModel::data(const QModelIndex &index, int role) const {
    switch (role) {
        case Qt::DisplayRole:
            if(m_temporarySearch == "") {
                return m_history[index.row()].value;
            }
            else if(index.row() == 0) {
                return m_temporarySearch;
            }
            else{
                return m_history[index.row() - 1].value;
            }
    }
    
    Q_UNREACHABLE();
}

void SearchHistoryModel::addSearch(const QString& search) {
    auto itr = find_if(m_history.begin(), m_history.end(), [&](const auto &checkedValue) {
        return checkedValue.value == search;
    });
    if(itr == m_history.end()) {
        beginInsertRows({}, 0, 0);
        m_history.insert(m_history.begin(), {search});
        endInsertRows();
    }
}

const QString& SearchHistoryModel::temporarySearch() const
{
    return m_temporarySearch;
}

void SearchHistoryModel::setTemporarySearch(const QString& text)
{
    if(text == "" && m_temporarySearch != "") {
        beginRemoveRows(QModelIndex(), 0, 0);
        m_temporarySearch = text;
        endRemoveRows();
    }
    else if(text != "" && m_temporarySearch == "") {
        beginInsertRows(QModelIndex(), 0, 0);
        m_temporarySearch = text;
        endInsertRows();
    }
    else if(m_temporarySearch != "") {
        m_temporarySearch = text;
        Q_EMIT dataChanged(createIndex(0,0), createIndex(0,0));
    }
}




WasPlayedWatcher::WasPlayedWatcher(Library* library, const QString& videoId)
    : QObject(library), m_videoId(videoId), m_library(library)
{
    connect(m_library, &Library::playbackHistoryChanged, this, &WasPlayedWatcher::query);
    query();
}

void WasPlayedWatcher::query()
{
    QCoro::connect(m_library->database().getResult<SingleValue<bool>>("select count(*) > 0 from played_songs where video_id = ?", m_videoId), this, &WasPlayedWatcher::update);
}


bool WasPlayedWatcher::wasPlayed() const
{
    return m_wasPlayed;
}


void WasPlayedWatcher::update(std::optional<SingleValue<bool> > result)
{
    if(result.has_value())
    {
        m_wasPlayed = result->value;
        Q_EMIT wasPlayedChanged();
    }
}

LocalSearchModel::LocalSearchModel(QObject *parent) : PlaybackHistoryModel(parent)
{
    connect(this, &LocalSearchModel::searchQueryChanged, this, [this]() {
        auto resultFuture = Library::instance().database()
                                .getResults<PlayedSong>("select * from played_songs natural join songs "
                                                        "where title like '%" % m_searchQuery % "%' "
                                                        "order by plays desc limit 10");
        QCoro::connect(std::move(resultFuture), this, [this](auto results) {
               beginResetModel();
               m_playedSongs = results;
               endResetModel();
           });
    });
}
