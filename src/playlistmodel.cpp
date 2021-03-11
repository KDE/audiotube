// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "playlistmodel.h"

#include <asyncytmusic.h>

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(this, &PlaylistModel::initialVideoIdChanged, this, [=] {
        if (m_initialVideoId.isEmpty()) {
            return;
        }

        setLoading(true);
        AsyncYTMusic::instance().fetchWatchPlaylist(m_initialVideoId);
    });
    connect(&AsyncYTMusic::instance(), &AsyncYTMusic::fetchWatchPlaylistFinished, this, [=](const watch::Playlist &playlist) {
        setLoading(false);

        beginResetModel();
        m_playlist = playlist;
        m_currentVideoId = m_initialVideoId;
        endResetModel();

        currentVideoIdChanged();
    });
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_playlist.tracks.size();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Title:
        return QString::fromStdString(m_playlist.tracks[index.row()].title);
    case VideoId:
        return QString::fromStdString(m_playlist.tracks[index.row()].video_id);
    case Artists: {
        const auto artists = m_playlist.tracks[index.row()].artists;
        QStringList artistNames;
        std::transform(artists.begin(), artists.end(), std::back_inserter(artistNames), [](const meta::Artist &artist) {
            return QString::fromStdString(artist.name);
        });
        return artistNames.join(", ");
    }
    case IsCurrent:
        return m_playlist.tracks[index.row()].video_id == m_currentVideoId.toStdString();
    }

    Q_UNREACHABLE();

    return {};
}

QHash<int, QByteArray> PlaylistModel::roleNames() const
{
    return {
        {Title, "title"},
        {VideoId, "videoId"},
        {Artists, "artists"},
        {IsCurrent, "isCurrent"}
    };
}

QString PlaylistModel::initialVideoId() const
{
    return m_initialVideoId;
}

void PlaylistModel::setInitialVideoId(const QString &videoId)
{
    m_initialVideoId = videoId;
    Q_EMIT initialVideoIdChanged();
}

bool PlaylistModel::loading() const
{
    return m_loading;
}

void PlaylistModel::setLoading(bool loading)
{
    m_loading = loading;
    Q_EMIT loadingChanged();
}

QString PlaylistModel::nextVideoId() const
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

QString PlaylistModel::currentVideoId() const
{
    return m_currentVideoId;
}

void PlaylistModel::setCurrentVideoId(const QString &videoId)
{
    const auto old = m_currentVideoId;
    m_currentVideoId = videoId;
    emitCurrentVideoChanged(old);
    Q_EMIT currentVideoIdChanged();
}

void PlaylistModel::next()
{
    const auto old = m_currentVideoId;
    m_currentVideoId = nextVideoId();
    emitCurrentVideoChanged(old);
    Q_EMIT currentVideoIdChanged();
}

void PlaylistModel::skipTo(const QString &videoId)
{
    const auto old = m_currentVideoId;
    m_currentVideoId = videoId;
    emitCurrentVideoChanged(old);
    Q_EMIT currentVideoIdChanged();
}

void PlaylistModel::emitCurrentVideoChanged(const QString &oldVideoId)
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

    dataChanged(index(oldIndex), index(oldIndex), {IsCurrent});
    dataChanged(index(newIndex), index(newIndex), {IsCurrent});
}
