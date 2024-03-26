// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QAbstractListModel>

#include <vector>

struct PlaylistEntry {
    using ColumnTypes = std::tuple<QString, QString, QString, QString>;

    QString videoId;
    QString title;
    QString artists;
    QString album;
};

class LocalPlaylistModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString playlistId READ playlistId WRITE setPlaylistId NOTIFY playlistIdChanged)

    enum Roles {
        VideoId = Qt::UserRole + 1,
        Title,
        Artists,
        ArtistsDisplayString
    };

public:
    LocalPlaylistModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;

    QString playlistId() const;
    void setPlaylistId(const QString &playlistId);
    Q_SIGNAL void playlistIdChanged();
    Q_INVOKABLE void removeSong(QString videoId, qint64 playlistId);

    void refreshModel();
    const std::vector<PlaylistEntry> &entries() const;

private:
    QString m_playlistId;
    std::vector<PlaylistEntry> m_entries;
};

