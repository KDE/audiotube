// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "playlistmodel.h"

#include <asyncytmusic.h>

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(this, &PlaylistModel::videoIdChanged, this, [=] {
        if (m_videoId.isEmpty()) {
            return;
        }

        setLoading(true);
        AsyncYTMusic::instance().fetchWatchPlaylist(m_videoId);
    });
    connect(&AsyncYTMusic::instance(), &AsyncYTMusic::fetchWatchPlaylistFinished, this, [=](const watch::Playlist &playlist) {
        setLoading(false);

        beginResetModel();
        m_playlist = playlist;
        endResetModel();
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
    case Artists:
        const auto artists = m_playlist.tracks[index.row()].artists;
        QStringList artistNames;
        std::transform(artists.begin(), artists.end(), std::back_inserter(artistNames), [](const meta::Artist &artist) {
            return QString::fromStdString(artist.name);
        });
        return artistNames.join(", ");
    }

    return {};
}

QHash<int, QByteArray> PlaylistModel::roleNames() const
{
    return {
        {Title, "title"},
        {VideoId, "videoId"},
        {Artists, "artists"}
    };
}

QString PlaylistModel::videoId() const
{
    return m_videoId;
}

void PlaylistModel::setVideoId(const QString &videoId)
{
    m_videoId = videoId;
    Q_EMIT videoIdChanged();
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
