// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "localplaylistsmodel.h"

#include "library.h"

LocalPlaylistsModel::LocalPlaylistsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(&Library::instance(), &Library::playlistsChanged,
            this, &LocalPlaylistsModel::refreshModel);
    refreshModel();
}

int LocalPlaylistsModel::rowCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : m_playlists.size();
}

QHash<int, QByteArray> LocalPlaylistsModel::roleNames() const
{
    return {
        {Roles::PlaylistId, "playlistId"},
        {Roles::Title, "title"},
        {Roles::Description, "description"},
        {Roles::CreatedOn, "createdOn"}
    };
}

QVariant LocalPlaylistsModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Roles::PlaylistId:
        return m_playlists[index.row()].playlistId;
    case Roles::Title:
        return m_playlists[index.row()].title;
    case Roles::Description:
        return m_playlists[index.row()].description;
    case Roles::CreatedOn:
        return m_playlists[index.row()].createdOn;
    }

    Q_UNREACHABLE();
}

void LocalPlaylistsModel::refreshModel()
{
    connectFuture(Library::instance().database().getResults<Playlist>("select * from playlists"), this, [this](auto playlists) {
        beginResetModel();
        m_playlists = playlists;
        endResetModel();
    });
}
