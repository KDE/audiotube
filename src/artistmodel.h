// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QAbstractListModel>
#include <QUrl>

#include <optional>

#include <ytmusic.h>
#include "multiiterableview.h"

class ArtistModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString channelId READ channelId WRITE setChannelId NOTIFY channelIdChanged REQUIRED)

    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QUrl thumbnailUrl READ thumbnailUrl NOTIFY thumbnailUrlChanged)
    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)

public:
    enum Type {
        Song,
        Album,
        Single,
        Video
    };
    Q_ENUM(Type)

    enum Role {
        Title,
        Type,
        Artists,
        VideoId
    };

    explicit ArtistModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString channelId() const;
    void setChannelId(const QString &channelId);
    Q_SIGNAL void channelIdChanged();

    QString title() const;
    Q_SIGNAL void titleChanged();

    QUrl thumbnailUrl() const;
    Q_SIGNAL void thumbnailUrlChanged();

    bool loading() const;
    void setLoading(bool loading);
    Q_SIGNAL void loadingChanged();

    Q_INVOKABLE void triggerItem(int row);

    Q_SIGNAL void openAlbum(const QString &browseId);
    Q_SIGNAL void openSong(const QString &videoId);
    Q_SIGNAL void openVideo(const QString &videoId);

private:
    QString m_channelId;
    bool m_loading = false;

    artist::Artist m_artist;
    std::optional<MultiIterableView<
        artist::Artist::Album, artist::Artist::Single, artist::Artist::Song, artist::Artist::Video
    >> m_view;
};
