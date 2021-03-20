// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QAbstractListModel>
#include <ytmusic.h>

class PlaylistModel : public QAbstractListModel
{
    Q_OBJECT

    // input
    Q_PROPERTY(QString playlistId READ playlistId WRITE setPlaylistId NOTIFY playlistIdChanged REQUIRED)

    // output
    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)
    Q_PROPERTY(QUrl thumbnailUrl READ thumbnailUrl NOTIFY thumbnailUrlChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)

public:
    explicit PlaylistModel(QObject *parent = nullptr);

    enum Role {
        Title,
        Artists,
        VideoId
    };

    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;

    QString playlistId() const;
    void setPlaylistId(const QString &playlistId);
    Q_SIGNAL void playlistIdChanged();

    bool loading() const;
    void setLoading(bool loading);
    Q_SIGNAL void loadingChanged();

    QUrl thumbnailUrl() const;
    Q_SIGNAL void thumbnailUrlChanged();

    QString title() const;
    Q_SIGNAL void titleChanged();

private:
    QString m_playlistId;
    bool m_loading = false;

    playlist::Playlist m_playlist {};
};
