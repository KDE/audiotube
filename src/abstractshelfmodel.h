// SPDX-FileCopyrightText: 2026 Tanveer Ahmed Mansuri
// SPDX-FileCopyrightText: 2026 Carl Schwan <carlschwan@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include "abstractytmusicmodel.h"
#include "ytmusic.h"
#include "searchmodel.h" 

// Model for items within a shelf (horizontal list)
class HomeShelfModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
public:
    enum Role {
        Title = Qt::UserRole + 1,
        TypeRole,
        VideoId,
        Artists,
        RadioPlaylistId,
        ThumbnailUrl,
        ArtistsDisplayString
    };
    Q_ENUM(Role)

    explicit HomeShelfModel(QObject *parent = nullptr);
    void setItems(std::vector<search::SearchResultItem> items);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void triggerItem(int row);

    Q_SIGNAL void openAlbum(const QString &browseId);
    Q_SIGNAL void openArtist(const QString &browseId, const QString &radioId, const QString &shuffleId);
    Q_SIGNAL void openPlaylist(const QString &browseId);
    Q_SIGNAL void openSong(const QString &videoId);
    Q_SIGNAL void openVideo(const QString &videoId, const QString &title);
    Q_SIGNAL void openMood(const QString &title, const QString &params);

private:
    std::vector<search::SearchResultItem> m_items;
};

class AbstractShelfModel : public AbstractYTMusicModel {
    Q_OBJECT

public:
    enum Role {
        Title = Qt::UserRole + 1,
        ContentModelRole
    };

    explicit AbstractShelfModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Re-expose signals from shelf models
    Q_SIGNAL void openAlbum(const QString &browseId);
    Q_SIGNAL void openArtist(const QString &browseId, const QString &radioId, const QString &shuffleId);
    Q_SIGNAL void openPlaylist(const QString &browseId);
    Q_SIGNAL void openSong(const QString &videoId);
    Q_SIGNAL void openVideo(const QString &videoId, const QString &title);
    Q_SIGNAL void openMood(const QString &title, const QString &params);

protected:
    void addShelf(const std::vector<search::SearchResultItem> &contents);

    std::vector<home::Shelf> m_shelves;
    std::vector<std::unique_ptr<HomeShelfModel>> m_shelfModels;
};
