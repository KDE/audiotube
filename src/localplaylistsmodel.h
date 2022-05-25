// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QAbstractListModel>
#include <QDateTime>

struct Playlist {
    using ColumnTypes = std::tuple<QString, QString, QString, QDateTime>;

    Playlist static fromSql(ColumnTypes tuple) {
        auto [playlistId, title, description, createdOn] = tuple;
        return Playlist { playlistId, title, description, createdOn };
    }

    QString playlistId;
    QString title;
    QString description;
    QDateTime createdOn;
};

class LocalPlaylistsModel : public QAbstractListModel
{
    Q_OBJECT

    enum Roles {
        PlaylistId,
        Title,
        Description,
        CreatedOn
    };

public:
    LocalPlaylistsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;

    void refreshModel();

private:
    std::vector<Playlist> m_playlists;
};

