// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include "ytmusic.h"
#include "playlistimporter.h"
#include <QAbstractListModel>
#include <QDateTime>

#include <ThreadedDatabase>

struct Playlist {
    using ColumnTypes = std::tuple<qint64, QString, QString, QDateTime>;

    Playlist static fromSql(ColumnTypes tuple) {
        auto [playlistId, title, description, createdOn] = tuple;
        return Playlist { playlistId, title, description, createdOn };
    }

    qint64 playlistId;
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
        CreatedOn,
        ThumbnailIds
    };

public:
    LocalPlaylistsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;

    Q_INVOKABLE void addPlaylist(const QString &title, const QString &description);
    Q_INVOKABLE void addPlaylistEntry(qint64 playlistId, const QString &videoId, const QString &title, const QString &artist, const QString &album);
    Q_INVOKABLE void addPlaylistEntry(qint64 playlistId, const playlist::Track &track);
    Q_INVOKABLE void importPlaylist(const QString &url);
    Q_SIGNAL void importFinished();

    Q_INVOKABLE void renamePlaylist(qint64 playlistId, const QString &name, const QString &description);
    Q_INVOKABLE void deletePlaylist(qint64 playlistId);


    Q_SIGNAL void playlistEntriesChanged(quint64 playlistId);

    void refreshModel();

private:
    QStringView cropURL(QStringView srcURL);
    std::vector<Playlist> m_playlists;
    std::vector<std::vector<QString>> m_thumbnailIds;
    PlaylistImporter *importer;
};

