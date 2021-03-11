// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <QAbstractListModel>

#include <ytmusic.h>

class PlaylistModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString initialVideoId READ initialVideoId WRITE setInitialVideoId NOTIFY initialVideoIdChanged)

    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)
    Q_PROPERTY(QString currentVideoId READ currentVideoId NOTIFY currentVideoIdChanged)

public:
    enum Role {
        Title = Qt::UserRole + 1,
        VideoId,
        Artists
    };

    explicit PlaylistModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString initialVideoId() const;
    void setInitialVideoId(const QString &videoId);
    Q_SIGNAL void initialVideoIdChanged();

    bool loading() const;
    void setLoading(bool loading);
    Q_SIGNAL void loadingChanged();

    QString nextVideoId() const;

    QString currentVideoId() const;
    Q_SIGNAL void currentVideoIdChanged();

    Q_INVOKABLE void next();

private:
    QString m_initialVideoId;
    QString m_currentVideoId;
    bool m_loading = false;

    watch::Playlist m_playlist;
};