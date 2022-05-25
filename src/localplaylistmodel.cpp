// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "localplaylistmodel.h"

#include "library.h"

LocalPlaylistModel::LocalPlaylistModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(this, &LocalPlaylistModel::playlistIdChanged,
            this, &LocalPlaylistModel::refreshModel);
    connect(&Library::instance(), &Library::playlistEntriesChanged,
            this, &LocalPlaylistModel::refreshModel);
}

int LocalPlaylistModel::rowCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : m_entries.size();
}

QHash<int, QByteArray> LocalPlaylistModel::roleNames() const
{
    return {
        { Roles::VideoId, "videoId" }
    };
}

QVariant LocalPlaylistModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        return m_entries[index.row()].title;
    case Roles::VideoId:
        return m_entries[index.row()].videoId;
    }

    Q_UNREACHABLE();
}

QString LocalPlaylistModel::playlistId() const
{
    return m_playlistId;
}

void LocalPlaylistModel::setPlaylistId(const QString &playlistId)
{
    m_playlistId = playlistId;
    Q_EMIT playlistIdChanged();
}

void LocalPlaylistModel::refreshModel()
{
    auto future = Library::instance()
            .database()
            .getResults<PlaylistEntry>(
                "select (video_id, title) from "
                "playlists_entries natural join songs where playlist_id = ?", m_playlistId);

    connectFuture(future, this, [this](auto entries) {
        beginResetModel();
        m_entries = entries;
        endResetModel();
    });
}
