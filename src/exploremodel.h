// SPDX-FileCopyrightText: 2026 Tanveer Ahmed Mansuri
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QAbstractListModel>
#include <qqmlregistration.h>
#include <memory>

#include "abstractytmusicmodel.h"
#include "ytmusic.h"
#include "homemodel.h" 
#include "abstractshelfmodel.h" 

// Main model for the Explore tab
class ExploreModel : public AbstractShelfModel {
    Q_OBJECT
    QML_ELEMENT

public:
    enum Role {
        Title = Qt::UserRole + 1,
        ContentModelRole
    };

    explicit ExploreModel(QObject *parent = nullptr);

    void refresh();
};

// Model for Mood Playlists
class MoodPlaylistsModel : public AbstractYTMusicModel {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString params READ params WRITE setParams NOTIFY paramsChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)

public:
    enum Role {
        Title = Qt::UserRole + 1,
        TypeRole,
        VideoId,
        Artists,
        ThumbnailUrl,
        ItemCount, // For playlists
        Author     // For playlists
    };
    Q_ENUM(Role)

    explicit MoodPlaylistsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString params() const;
    void setParams(const QString &params);
    Q_SIGNAL void paramsChanged();

    QString title() const;
    void setTitle(const QString &title);
    Q_SIGNAL void titleChanged();

    Q_INVOKABLE void triggerItem(int row);

    Q_SIGNAL void openPlaylist(const QString &browseId);

private:
    QString m_params;
    QString m_title;
    std::vector<search::SearchResultItem> m_items;
    void load();
};
