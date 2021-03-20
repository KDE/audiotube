// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "userplaylistmodel.h"

#include <asyncytmusic.h>

#include "playlistutils.h"

UserPlaylistModel::UserPlaylistModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(this, &UserPlaylistModel::initialVideoIdChanged, this, [=] {
        if (m_initialVideoId.isEmpty()) {
            return;
        }

        setLoading(true);
        YTMusicThread::instance()->fetchWatchPlaylist(m_initialVideoId);
    });
    connect(this, &UserPlaylistModel::playlistIdChanged, this, [=] {
        if (m_playlistId.isEmpty()) {
            return;
        }

        setLoading(true);
        YTMusicThread::instance()->fetchWatchPlaylist(std::nullopt, m_playlistId);
    });
    connect(&YTMusicThread::instance().get(), &AsyncYTMusic::fetchWatchPlaylistFinished, this, [=](const watch::Playlist &playlist) {
        setLoading(false);

        beginResetModel();
        m_playlist = playlist;
        if (!m_playlist.tracks.empty()) {
            setCurrentVideoId(QString::fromStdString(m_playlist.tracks.front().video_id));
        } else {
            setCurrentVideoId({});
        }
        endResetModel();
    });
    connect(&YTMusicThread::instance().get(), &AsyncYTMusic::errorOccurred, this, [=] {
        setLoading(false);
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
        {IsCurrent, "isCurrent"}
    };
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

bool UserPlaylistModel::loading() const
{
    return m_loading;
}

void UserPlaylistModel::setLoading(bool loading)
{
    m_loading = loading;
    Q_EMIT loadingChanged();
}

QString UserPlaylistModel::nextVideoId() const
{
    auto currentTrack = std::find_if(m_playlist.tracks.begin(), m_playlist.tracks.end(),
                                     [=](const watch::Playlist::Track &track) {
        return track.video_id == m_currentVideoId.toStdString();
    });

    if (currentTrack == m_playlist.tracks.end() || currentTrack + 1 == m_playlist.tracks.end()) {
        return {};
    }

    return QString::fromStdString((currentTrack + 1)->video_id);
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
}

bool UserPlaylistModel::canSkip() const
{
    const auto currentTrackIt = std::find_if(m_playlist.tracks.begin(), m_playlist.tracks.end(),
                                        [=](const watch::Playlist::Track &track) {
        return track.video_id == m_currentVideoId.toStdString();
    });

    return currentTrackIt != m_playlist.tracks.end() - 1 && !m_playlist.tracks.empty();
}

void UserPlaylistModel::next()
{
    setCurrentVideoId(nextVideoId());
    Q_EMIT currentVideoIdChanged();
    Q_EMIT canSkipChanged();
}

void UserPlaylistModel::skipTo(const QString &videoId)
{
    const auto old = m_currentVideoId;
    m_currentVideoId = videoId;
    emitCurrentVideoChanged(old);
    Q_EMIT currentVideoIdChanged();
    Q_EMIT canSkipChanged();
}

void UserPlaylistModel::playNext(const QString &videoId, const QString &title, const std::vector<meta::Artist> &artists)
{
    const auto currentIt = std::find_if(m_playlist.tracks.begin(), m_playlist.tracks.end(),
                                        [=](const watch::Playlist::Track &track) {
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
        return;
    }

    if (currentIt == m_playlist.tracks.end() - 1) {
        int index = std::distance(m_playlist.tracks.begin(), currentIt + 1);
        beginInsertRows({}, index, index);
        m_playlist.tracks.push_back(std::move(track));
        endInsertRows();
        Q_EMIT canSkipChanged();
        return;
    }

    int index = std::distance(m_playlist.tracks.begin(), currentIt + 1);
    beginInsertRows({}, index, index);
    m_playlist.tracks.insert(currentIt + 1, std::move(track));
    endInsertRows();
    Q_EMIT canSkipChanged();
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
}

void UserPlaylistModel::clear()
{
    beginResetModel();
    m_playlist.tracks.clear();
    endResetModel();

    setCurrentVideoId({});
    Q_EMIT canSkipChanged();
}

void UserPlaylistModel::remove(const QString &videoId)
{
    if (m_currentVideoId == videoId) {
        setCurrentVideoId(nextVideoId());
    }

    const auto trackIt = std::find_if(m_playlist.tracks.begin(), m_playlist.tracks.end(), [&](const watch::Playlist::Track &track) {
        return track.video_id == videoId.toStdString();
    });
    int index = std::distance(m_playlist.tracks.begin(), trackIt);
    beginRemoveRows({}, index, index);
    m_playlist.tracks.erase(trackIt);
    endRemoveRows();

    Q_EMIT canSkipChanged();
}

void UserPlaylistModel::emitCurrentVideoChanged(const QString &oldVideoId)
{
    const auto oldVideoIt = std::find_if(m_playlist.tracks.begin(), m_playlist.tracks.end(),
                                             [=](const watch::Playlist::Track &track) {
        return track.video_id == oldVideoId.toStdString();
    });
    const auto currentVideoIt = std::find_if(m_playlist.tracks.begin(), m_playlist.tracks.end(),
                                             [=](const watch::Playlist::Track &track) {
        return track.video_id == m_currentVideoId.toStdString();
    });

    int oldIndex = std::distance(m_playlist.tracks.begin(), oldVideoIt);
    int newIndex = std::distance(m_playlist.tracks.begin(), currentVideoIt);

    Q_EMIT dataChanged(index(oldIndex), index(oldIndex), {IsCurrent});
    Q_EMIT dataChanged(index(newIndex), index(newIndex), {IsCurrent});
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