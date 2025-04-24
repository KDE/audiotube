// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QAbstractListModel>
#include <qqmlintegration.h>
#include <ytmusic.h>

#include "abstractytmusicmodel.h"

class PlaylistModel : public AbstractYTMusicModel
{
    Q_OBJECT
    QML_ELEMENT

    // input
    Q_PROPERTY(QString playlistId READ playlistId WRITE setPlaylistId NOTIFY playlistIdChanged REQUIRED)

    // output
    Q_PROPERTY(QUrl thumbnailUrl READ thumbnailUrl NOTIFY thumbnailUrlChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QUrl webUrl READ webUrl NOTIFY playlistIdChanged)


public:
    explicit PlaylistModel(QObject *parent = nullptr);

    enum Role {
        Title,
        Artists,
        VideoId,
        ThumbnailUrl,
        ArtistsDisplayString
    };

    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;

    QString playlistId() const;
    void setPlaylistId(const QString &playlistId);
    Q_SIGNAL void playlistIdChanged();

    QUrl thumbnailUrl() const;
    Q_SIGNAL void thumbnailUrlChanged();

    QString title() const;
    Q_SIGNAL void titleChanged();

    QUrl webUrl() const;

    playlist::Playlist playlist() const;

private:
    QString m_playlistId;

    playlist::Playlist m_playlist {};
};
