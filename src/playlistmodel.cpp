// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL


#include "playlistmodel.h"

#include <QUrl>

#include "asyncytmusic.h"
#include "playlistutils.h"

PlaylistModel::PlaylistModel(QObject *parent)
    : AbstractYTMusicModel(parent)
{
    connect(this, &PlaylistModel::playlistIdChanged, this, [=, this] {
        setLoading(true);
        auto future = YTMusicThread::instance()->fetchPlaylist(m_playlistId);
        connectFuture(future, this, [=, this](const playlist::Playlist &playlist) {
            setLoading(false);
            beginResetModel();
            m_playlist = playlist;
            std::sort(m_playlist.thumbnails.begin(), m_playlist.thumbnails.end());
            endResetModel();

            Q_EMIT titleChanged();
            Q_EMIT thumbnailUrlChanged();
        });
    });
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : int(m_playlist.tracks.size());
}

QHash<int, QByteArray> PlaylistModel::roleNames() const
{
    return {
        {Title, "title"},
        {Artists, "artists"},
        {VideoId, "videoId"}
    };
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Title:
        return QString::fromStdString(m_playlist.tracks[index.row()].title);
    case Artists:
        return QVariant::fromValue(m_playlist.tracks[index.row()].artists);
    case VideoId:
        if (m_playlist.tracks[index.row()].video_id) {
            return QString::fromStdString(*m_playlist.tracks[index.row()].video_id);
        }
        return {};
    }

    Q_UNREACHABLE();

    return {};
}

QString PlaylistModel::playlistId() const
{
    auto id = m_playlistId;
    id.remove(QStringLiteral("VL")); // Workaround: get_watch_playlist only accepts
                                     // the playlists without the leading VL
    return id;
}

void PlaylistModel::setPlaylistId(const QString &playlistId)
{
    m_playlistId = playlistId;
    Q_EMIT playlistIdChanged();
}

QUrl PlaylistModel::thumbnailUrl() const
{
    if (m_playlist.thumbnails.empty()) {
        return QUrl();
    }

    return QUrl(QString::fromStdString(m_playlist.thumbnails.back().url));
}

QString PlaylistModel::title() const
{
    return QString::fromStdString(m_playlist.title);
}
