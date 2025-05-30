// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QAbstractListModel>
#include <QUrl>
#include <qqmlintegration.h>

#include "asyncytmusic.h"
#include "abstractytmusicmodel.h"

class AlbumModel : public AbstractYTMusicModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString browseId READ browseId WRITE setBrowseId NOTIFY browseIdChanged REQUIRED)

    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString artists READ artists NOTIFY artistsChanged)
    Q_PROPERTY(QUrl thumbnailUrl READ thumbnailUrl NOTIFY thumbnailUrlChanged)
    Q_PROPERTY(QString playlistId READ playlistId NOTIFY playlistIdChanged)
    Q_PROPERTY(QUrl webUrl READ webUrl NOTIFY playlistIdChanged)

public:
    enum Role {
        Title = Qt::UserRole + 1,
        VideoId,
        Artists,
        ThumbnailUrl,
        ArtistsDisplayString
    };

    explicit AlbumModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString browseId() const;
    void setBrowseId(const QString &value);
    Q_SIGNAL void browseIdChanged();

    QString title() const;
    Q_SIGNAL void titleChanged();

    QString artists() const;
    Q_SIGNAL void artistsChanged();
    QUrl thumbnailUrl() const;
    Q_SIGNAL void thumbnailUrlChanged();

    QString playlistId() const;
    Q_SIGNAL void playlistIdChanged();

    QUrl webUrl() const;

    const album::Album &album() const;

private:
    QString m_browseId;

    album::Album m_album;
};
