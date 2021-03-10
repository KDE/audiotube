// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QThread>

#include "asyncytmusic.h"

class SearchModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString searchQuery READ searchQuery WRITE setSearchQuery NOTIFY searchQueryChanged)
    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)

public:
    enum Type {
        Artist,
        Album,
        Playlist,
        Song,
        Video
    };
    Q_ENUM(Type)

    enum Role {
        Title = Qt::UserRole + 1,
        Type
    };

    explicit SearchModel(QObject *parent = nullptr);
    ~SearchModel();

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString searchQuery() const;
    void setSearchQuery(const QString &searchQuery);
    Q_SIGNAL void searchQueryChanged();

    bool loading() const;
    void setLoading(bool loading);
    Q_SIGNAL void loadingChanged();

    Q_INVOKABLE void triggerItem(int row);

    Q_SIGNAL void openAlbum(const QString &browseId);
    Q_SIGNAL void openArtist(const QString &browseId);
    Q_SIGNAL void openPlaylist(const QString &browseId);
    Q_SIGNAL void openSong(const QString &videoId);
    Q_SIGNAL void openVideo(const QString &videoId);

private:
    QString m_searchQuery;
    std::vector<search::SearchResultItem> m_searchResults;
    bool m_loading = false;
};
