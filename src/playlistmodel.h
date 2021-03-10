// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <QAbstractListModel>

#include <ytmusic.h>

class PlaylistModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString videoId READ videoId WRITE setVideoId NOTIFY videoIdChanged)
    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)

public:
    enum Role {
        Title,
        VideoId,
        Artists
    };

    explicit PlaylistModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString videoId() const;
    void setVideoId(const QString &videoId);
    Q_SIGNAL void videoIdChanged();

    bool loading() const;
    void setLoading(bool loading);
    Q_SIGNAL void loadingChanged();

private:
    QString m_videoId;
    bool m_loading = false;

    watch::Playlist m_playlist;
};
