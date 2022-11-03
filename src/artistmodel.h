// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QAbstractListModel>
#include <QUrl>

#include <optional>

#include <ytmusic.h>

#include "multiiterableview.h"
#include "abstractytmusicmodel.h"


class ArtistModel : public AbstractYTMusicModel
{
    Q_OBJECT

    Q_PROPERTY(QString channelId READ channelId WRITE setChannelId NOTIFY channelIdChanged REQUIRED)

    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QUrl thumbnailUrl READ thumbnailUrl NOTIFY thumbnailUrlChanged)
    Q_PROPERTY(QUrl webUrl READ webUrl NOTIFY webUrlChanged)

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
        VideoId,
        ThumbnailUrl
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

    QUrl webUrl() const;
    Q_SIGNAL void webUrlChanged();

    Q_INVOKABLE void triggerItem(int row);

    Q_SIGNAL void openAlbum(const QString &browseId);
    Q_SIGNAL void openSong(const QString &videoId);
    Q_SIGNAL void openVideo(const QString &videoId, const QString &title);

private:
    QString m_channelId;

    artist::Artist m_artist;

    std::vector<artist::Artist::Album> albums;
    std::vector<artist::Artist::Single> singles;
    std::vector<artist::Artist::Song> songs;
    std::vector<artist::Artist::Video> videos;

    MultiIterableView<
        artist::Artist::Album, artist::Artist::Single, artist::Artist::Song, artist::Artist::Video
    > m_view;
};
