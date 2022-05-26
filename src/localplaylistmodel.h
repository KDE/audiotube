// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QAbstractListModel>

#include <vector>

struct PlaylistEntry {
    using ColumnTypes = std::tuple<QString, QString>;

    static PlaylistEntry fromSql(ColumnTypes tuple) {
        auto [videoId, title] = tuple;
        return PlaylistEntry { videoId, title };
    }

    QString videoId;
    QString title;
};

class LocalPlaylistModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString playlistId READ playlistId WRITE setPlaylistId NOTIFY playlistIdChanged)

    enum Roles {
        VideoId = Qt::UserRole + 1,
        Title
    };

public:
    LocalPlaylistModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;

    QString playlistId() const;
    void setPlaylistId(const QString &playlistId);
    Q_SIGNAL void playlistIdChanged();

    void refreshModel();

private:
    QString m_playlistId;
    std::vector<PlaylistEntry> m_entries;
};

