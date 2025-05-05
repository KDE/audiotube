// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "userplaylistmodel.h"

#include <asyncytmusic.h>
#include <algorithm>

#include <QRegularExpression>
#include <QRandomGenerator>

#include <QStringBuilder>

#include "albummodel.h"
#include "localplaylistmodel.h"
#include "playlistutils.h"
#include "playlistmodel.h"

namespace ranges = std::ranges;
using namespace Qt::Literals::StringLiterals;

UserPlaylistModel::UserPlaylistModel(QObject *parent)
    : AbstractYTMusicModel(parent)
{
    auto handleResult = [=, this](watch::Playlist &&playlist) {
        setLoading(false);

        beginResetModel();
        m_playlist = std::move(playlist);
        endResetModel();
        setCurrentVideoId({});
        if (m_shuffle) {
            shufflePlaylist();

            // reset shuffle
            setShuffle(false);
        }
        if (!m_playlist.tracks.empty()) {
            setCurrentVideoId(QString::fromStdString(m_playlist.tracks.front().video_id));
        }
    };
    connect(this, &UserPlaylistModel::initialVideoIdChanged, this, [=, this] {
        if (m_initialVideoId.isEmpty()) {
            return;
        }

        setLoading(true);
        auto future = YTMusicThread::instance()->fetchWatchPlaylist(m_initialVideoId);
        QCoro::connect(std::move(future), this, handleResult);
    });
    connect(this, &UserPlaylistModel::playlistIdChanged, this, [=, this] {
        if (m_playlistId.isEmpty()) {
            return;
        }

        setLoading(true);
        auto future = YTMusicThread::instance()->fetchWatchPlaylist(std::nullopt, m_playlistId);
        QCoro::connect(std::move(future), this, handleResult);;
    });
    connect(&YTMusicThread::instance().get(), &AsyncYTMusic::errorOccurred, this, [this] {
        setLoading(false);
    });
    connect(this, &UserPlaylistModel::currentVideoIdChanged, this, [this]() {
        Q_EMIT currentIndexChanged();
        // Clear lyrics, so we won't display old ones if the next song doesn't have any.
        m_lyrics = {};
        Q_EMIT lyricsChanged();

        if (!m_currentVideoId.isEmpty()) {
            fetchLyrics(m_currentVideoId);
        }
    });
}

int UserPlaylistModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : int(m_playlist.tracks.size());
}

QVariant UserPlaylistModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Title:
        return QString::fromStdString(m_playlist.tracks[index.row()].title);
    case VideoId:
        return QString::fromStdString(m_playlist.tracks[index.row()].video_id);
    case Artists:
        return PlaylistUtils::artistsToString(m_playlist.tracks[index.row()].artists);
    case Album:
        return QString::fromStdString(m_playlist.tracks[index.row()].album.value_or(meta::Album()).name);
    case AlbumId:
        return
        QString::fromStdString(m_playlist.tracks[index.row()].album.value_or(meta::Album()).id.value_or(""));
    case Duration:
    {
        auto length = QString::fromStdString(m_playlist.tracks[index.row()].length.value_or(""));
        QRegularExpression time{uR"((\d{1,2}):(\d{2}))"_s};
        auto match = time.match(length);
        int duration = match.captured(1).toInt() * 60 + match.captured(2).toInt();
        return duration;
    }
    case IsCurrent:
        return m_playlist.tracks[index.row()].video_id == m_currentVideoId.toStdString();
    }

    Q_UNREACHABLE();

    return {};
}

QHash<int, QByteArray> UserPlaylistModel::roleNames() const
{
    return {
        {Title, "title"},
        {VideoId, "videoId"},
        {Artists, "artists"},
        {Album, "album"},
        {IsCurrent, "isCurrent"},
    };
}

bool UserPlaylistModel::moveRow(int sourceRow,int destinationRow)
{
    if(beginMoveRows(QModelIndex(), sourceRow, sourceRow, QModelIndex(), destinationRow)) {
        m_playlist.tracks.insert(m_playlist.tracks.begin()+destinationRow, 1, m_playlist.tracks[sourceRow]);
        if(sourceRow < destinationRow) {
            m_playlist.tracks.erase(m_playlist.tracks.begin()+sourceRow);
        }
        else {
            m_playlist.tracks.erase(m_playlist.tracks.begin()+sourceRow+1);
        }
        endMoveRows();

        Q_EMIT currentIndexChanged();
        Q_EMIT canSkipChanged();
        Q_EMIT canSkipBackChanged();
        return true;
    }
    return false;
}

QString UserPlaylistModel::initialVideoId() const
{
    return m_initialVideoId;
}

void UserPlaylistModel::setInitialVideoId(const QString &videoId)
{
    m_initialVideoId = videoId;
    Q_EMIT initialVideoIdChanged();
}

QString UserPlaylistModel::nextVideoId() const
{
    auto currentTrack = std::find_if(m_playlist.tracks.begin(), m_playlist.tracks.end(),
                                     [this](const watch::Playlist::Track &track) {
        return track.video_id == m_currentVideoId.toStdString();
    });

    if (currentTrack == m_playlist.tracks.end() || currentTrack + 1 == m_playlist.tracks.end()) {
        return {};
    }

    return QString::fromStdString((currentTrack + 1)->video_id);
}

QString UserPlaylistModel::previousVideoId() const
{
    auto currentTrack = std::find_if(m_playlist.tracks.begin(), m_playlist.tracks.end(),
                                     [this](const watch::Playlist::Track &track) {
        return track.video_id == m_currentVideoId.toStdString();
    });

    if (currentTrack == m_playlist.tracks.end() || currentTrack - 1 == m_playlist.tracks.end()) {
        return {};
    }

    return QString::fromStdString((currentTrack - 1)->video_id);
}

QString UserPlaylistModel::currentVideoId() const
{
    return m_currentVideoId;
}

void UserPlaylistModel::setCurrentVideoId(const QString &videoId)
{
    const auto old = m_currentVideoId;
    m_currentVideoId = videoId;
    emitCurrentVideoChanged(old);
    Q_EMIT currentVideoIdChanged();
    Q_EMIT canSkipChanged();
    Q_EMIT canSkipBackChanged();

}

int UserPlaylistModel::currentIndex() const
{
    auto currentTrack = ranges::find_if(m_playlist.tracks,
                                        [this](const watch::Playlist::Track &track) {
        return track.video_id == m_currentVideoId.toStdString();
    });

    return std::distance(m_playlist.tracks.begin(), currentTrack);
}

bool UserPlaylistModel::canSkip() const
{
    const auto currentTrackIt = ranges::find_if(m_playlist.tracks,
                                                [this](const watch::Playlist::Track &track) {
        return track.video_id == m_currentVideoId.toStdString();
    });

    return currentTrackIt != m_playlist.tracks.end() - 1
            && currentTrackIt != m_playlist.tracks.end()
            && !m_playlist.tracks.empty();
}

bool UserPlaylistModel::canSkipBack() const
{
    const auto currentTrackIt = std::ranges::find_if(m_playlist.tracks, [this](const watch::Playlist::Track &track) {
        return track.video_id == m_currentVideoId.toStdString();
    });

    return currentTrackIt != m_playlist.tracks.begin()
            && !m_playlist.tracks.empty();
}

void UserPlaylistModel::next()
{
    setCurrentVideoId(nextVideoId());
    Q_EMIT currentVideoIdChanged();
    Q_EMIT canSkipChanged();
    Q_EMIT canSkipBackChanged();

}

void UserPlaylistModel::previous()
{
    setCurrentVideoId(previousVideoId());
    Q_EMIT currentVideoIdChanged();
    Q_EMIT canSkipChanged();
    Q_EMIT canSkipBackChanged();

}

void UserPlaylistModel::skipTo(const QString &videoId)
{
    const auto old = m_currentVideoId;
    m_currentVideoId = videoId;
    emitCurrentVideoChanged(old);
    Q_EMIT currentVideoIdChanged();
    Q_EMIT canSkipChanged();
    Q_EMIT canSkipBackChanged();

}

void UserPlaylistModel::playNext(const QString &videoId, const QString &title, const std::vector<meta::Artist> &artists)
{
    const auto currentIt = ranges::find_if(m_playlist.tracks,
                                        [this](const watch::Playlist::Track &track) {
        return track.video_id == m_currentVideoId.toStdString();
    });

    watch::Playlist::Track track;
    track.video_id = videoId.toStdString();
    track.title = title.toStdString();
    track.artists = artists;

    if (currentIt == m_playlist.tracks.end() || m_playlist.tracks.empty()) {
        beginInsertRows({}, 0, 0);
        m_playlist.tracks.push_back(std::move(track));
        endInsertRows();
        setCurrentVideoId(videoId);
        Q_EMIT canSkipChanged();
        Q_EMIT canSkipBackChanged();

        return;
    }

    if (currentIt == m_playlist.tracks.end() - 1) {
        int index = std::distance(m_playlist.tracks.begin(), currentIt + 1);
        beginInsertRows({}, index, index);
        m_playlist.tracks.push_back(std::move(track));
        endInsertRows();
        Q_EMIT canSkipChanged();
        Q_EMIT canSkipBackChanged();

        return;
    }

    int index = std::distance(m_playlist.tracks.begin(), currentIt + 1);
    beginInsertRows({}, index, index);
    m_playlist.tracks.insert(currentIt + 1, std::move(track));
    endInsertRows();
    Q_EMIT canSkipChanged();
    Q_EMIT canSkipBackChanged();

}

void UserPlaylistModel::append(const QString &videoId, const QString &title, const std::vector<meta::Artist> &artists)
{
    watch::Playlist::Track track;
    track.video_id = videoId.toStdString();
    track.title = title.toStdString();
    track.artists = artists;

    beginInsertRows({}, m_playlist.tracks.size(), m_playlist.tracks.size());
    m_playlist.tracks.push_back(std::move(track));
    endInsertRows();

    if (m_playlist.tracks.size() == 1) {
        setCurrentVideoId(videoId);
    }

    Q_EMIT canSkipChanged();
}

void UserPlaylistModel::clear()
{
    beginResetModel();
    m_playlist.tracks.clear();
    endResetModel();

    setCurrentVideoId({});
    Q_EMIT canSkipChanged();
    Q_EMIT canSkipBackChanged();
}

void UserPlaylistModel::clearExceptCurrent()
{
    int index = currentIndex();
    Q_ASSERT(checkIndex(createIndex(index, 0), CheckIndexOption::IndexIsValid | CheckIndexOption::DoNotUseParent));
    if(m_playlist.tracks.empty()) {return;}
    if((unsigned) index < m_playlist.tracks.size() - 1) {
        beginRemoveRows({}, index + 1, m_playlist.tracks.size() - 1);
        m_playlist.tracks.erase(m_playlist.tracks.begin() + index + 1, m_playlist.tracks.end());
        endRemoveRows();
    }
    
    if(index > 0) {
        beginRemoveRows({}, 0, index - 1);
        m_playlist.tracks.erase(m_playlist.tracks.begin(), m_playlist.tracks.begin() + index);
        endRemoveRows();
    }
    
    Q_EMIT canSkipChanged();
    Q_EMIT canSkipBackChanged();
}


void UserPlaylistModel::remove(const QString &videoId)
{
    if (m_currentVideoId == videoId) {
        setCurrentVideoId(nextVideoId());
    }

    const auto trackIt = ranges::find_if(m_playlist.tracks, [&](const watch::Playlist::Track &track) {
        return track.video_id == videoId.toStdString();
    });
    int index = std::distance(m_playlist.tracks.begin(), trackIt);
    beginRemoveRows({}, index, index);
    m_playlist.tracks.erase(trackIt);
    endRemoveRows();

    Q_EMIT canSkipChanged();
    Q_EMIT canSkipBackChanged();
}

void UserPlaylistModel::shufflePlaylist()
{
    // Only shuffle playlist after current track
    if (!m_currentVideoId.isEmpty()) {
        const auto currentIt = ranges::find_if(m_playlist.tracks,
                                               [this](const watch::Playlist::Track &track) {
            return track.video_id == m_currentVideoId.toStdString();
        });

        std::shuffle(currentIt + 1, m_playlist.tracks.end(), *QRandomGenerator::global());
    } else {
        ranges::shuffle(m_playlist.tracks, *QRandomGenerator::global());
    }
    Q_EMIT dataChanged(index(0), index(m_playlist.tracks.size() - 1), {});
}

void UserPlaylistModel::appendPlaylist(PlaylistModel *playlistModel)
{
    for (const auto &track : playlistModel->playlist().tracks) {
        if (track.video_id) {
            append(QString::fromStdString(*track.video_id), QString::fromStdString(track.title), track.artists);
        }
    }
}

void UserPlaylistModel::appendAlbum(AlbumModel *albumModel)
{
    for (const auto &track : albumModel->album().tracks) {
        if (track.video_id) {
            append(QString::fromStdString(*track.video_id), QString::fromStdString(track.title), track.artists);
        }
    }
}

void UserPlaylistModel::playFavourites(FavouritesModel *favouriteModel, bool shuffled)
{
    clear();
    appendFavourites(favouriteModel, shuffled);

}

void UserPlaylistModel::appendFavourites(FavouritesModel *favouriteModel, bool shuffled)
{
    std::vector<Song> favourites(favouriteModel->getFavouriteSongs());
    if(shuffled) {
        ranges::shuffle(favourites, *QRandomGenerator::global());
    }
    ranges::for_each(favourites, [this](const Song &song) {
        meta::Artist artist;
        artist.name = song.artist.toStdString();
        append(song.videoId, song.title, std::vector<meta::Artist>({artist}));
    });
}

void UserPlaylistModel::playPlaybackHistory(PlaybackHistoryModel *playbackHistory, bool shuffled)
{
    clear();
    appendPlaybackHistory(playbackHistory, shuffled);

}

void UserPlaylistModel::appendPlaybackHistory(PlaybackHistoryModel *playbackHistory, bool shuffled)
{
    std::vector<PlayedSong> playedSongs(playbackHistory->getPlayedSong());
    if(shuffled) {
        std::shuffle(playedSongs.begin(), playedSongs.end(), *QRandomGenerator::global());
    }
    ranges::for_each(playedSongs, [this](const PlayedSong &song) {
        meta::Artist artist;
        artist.name = song.artist.toStdString();
        append(song.videoId, song.title, std::vector<meta::Artist>({artist}));
    });
}

void UserPlaylistModel::playLocalPlaylist(LocalPlaylistModel *playlistModel, bool shuffled)
{
    clear();
    appendLocalPlaylist(playlistModel, shuffled);
}

void UserPlaylistModel::appendLocalPlaylist(LocalPlaylistModel *playlistModel, bool shuffled)
{
    std::vector<PlaylistEntry> entries = playlistModel->entries();
    if (shuffled) {
        ranges::shuffle(entries, *QRandomGenerator::global());
    }
    for (const auto &entry : entries) {
        meta::Artist artist;
        artist.name = entry.artists.toStdString();
        append(entry.videoId, entry.title, std::vector<meta::Artist>({artist}));
    }
}

void UserPlaylistModel::emitCurrentVideoChanged(const QString &oldVideoId)
{

    const auto oldVideoIt = ranges::find_if(m_playlist.tracks,
                                            [=](const watch::Playlist::Track &track) {
        return track.video_id == oldVideoId.toStdString();
    });
    const auto currentVideoIt = ranges::find_if(m_playlist.tracks,
                                                [this](const watch::Playlist::Track &track) {
        return track.video_id == m_currentVideoId.toStdString();
    });

    int oldIndex = std::distance(m_playlist.tracks.begin(), oldVideoIt);
    int newIndex = std::distance(m_playlist.tracks.begin(), currentVideoIt);

    Q_EMIT dataChanged(index(oldIndex), index(oldIndex), {IsCurrent});
    Q_EMIT dataChanged(index(newIndex), index(newIndex), {IsCurrent});
}

void UserPlaylistModel::fetchLyrics(const QString &videoId)
{
    auto future = YTMusicThread::instance()->fetchWatchPlaylist(videoId);
    QCoro::connect(std::move(future), this, [=, this](const auto &playlist) {
        if (playlist.lyrics) {
            QCoro::connect(YTMusicThread::instance()->fetchLyrics(QString::fromStdString(*playlist.lyrics)), this, [=, this](const auto &lyrics) {
                m_lyrics = lyrics;
                Q_EMIT lyricsChanged();
            });
        } else {
            Q_EMIT noLyrics();
        }
    });
}

bool UserPlaylistModel::shuffle() const
{
    return m_shuffle;
}

QString UserPlaylistModel::lyrics() const {
    return QString::fromStdString(m_lyrics.lyrics);
}

void UserPlaylistModel::setShuffle(bool shuffle)
{
    m_shuffle = shuffle;
    Q_EMIT shuffleChanged();
}

QString UserPlaylistModel::playlistId() const
{
    return m_playlistId;
}

void UserPlaylistModel::setPlaylistId(const QString &playlistId)
{
    m_playlistId = playlistId;
    Q_EMIT playlistIdChanged();
}

QUrl UserPlaylistModel::webUrl() const
{
    return QUrl(YTMUSIC_WEB_BASE_URL % u"watch?v=" % m_currentVideoId);
}

