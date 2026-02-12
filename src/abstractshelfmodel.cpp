// SPDX-FileCopyrightText: 2026 Tanveer Ahmed Mansuri
// SPDX-FileCopyrightText: 2026 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "abstractshelfmodel.h"
#include "playlistutils.h"
#include "asyncytmusic.h"
#include <QCoroFuture>

// --- HomeShelfModel ---

HomeShelfModel::HomeShelfModel(QObject *parent)
    : QAbstractListModel(parent) {}

void HomeShelfModel::setItems(std::vector<search::SearchResultItem> items) {
    beginResetModel();
    m_items = std::move(items);

    for (auto &item : m_items) {
        std::visit([&](auto &arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (!std::is_same_v<T, search::Mood>) {
                std::sort(arg.thumbnails.begin(), arg.thumbnails.end(), [](auto a, auto b) {
                    return a.width > b.width;
                });
            }
        }, item);
    }
    endResetModel();
}

int HomeShelfModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : int(m_items.size());
}

QVariant HomeShelfModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= int(m_items.size())) return {};

    const auto &item = m_items.at(index.row());

    switch (role) {
    case Title:
        return QString::fromStdString(std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, search::Album> || std::is_same_v<T, search::Playlist> || std::is_same_v<T, search::Song> || std::is_same_v<T, search::Video>) {
                return arg.title;
            } else if constexpr (std::is_same_v<T, search::Artist>) {
                return arg.artist;
            } else if constexpr (std::is_same_v<T, search::TopResult>) {
                 if (arg.title) return *arg.title;
                 if (!arg.artists.empty()) return arg.artists.front().name;
                 return std::string();
            } else if constexpr (std::is_same_v<T, search::Mood>) {
                return arg.title;
            } else {
                return std::string();
            }
        }, item));
    case TypeRole:
        // Use SearchModel::Type values for compatibility if needed, but we have our own enum if we want.
        // For simplicity, let's just return what SearchModel would return or map to strings if QML expects that.
        // Looking at SearchModel, it returns SearchModel::Type enum values.
        // But HomeShelfModel has its own enum for roles.
        // Wait, SearchModel::Type is registered in SearchModel. I should reuse SearchModel::Type if I want to use same delegates?
        // Actually, let's just use int values corresponding to SearchModel::Type to be safe, or cast.
        return std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, search::Album>) return SearchModel::Album;
            if constexpr (std::is_same_v<T, search::Artist>) return SearchModel::Artist;
            if constexpr (std::is_same_v<T, search::Playlist>) return SearchModel::Playlist;
            if constexpr (std::is_same_v<T, search::Song>) return SearchModel::Song;
            if constexpr (std::is_same_v<T, search::Video>) return SearchModel::Video;
            if constexpr (std::is_same_v<T, search::TopResult>) return SearchModel::TopResult;
            if constexpr (std::is_same_v<T, search::Mood>) return SearchModel::Mood;
            return SearchModel::Song; // Default?
        }, item);
    case VideoId:
        return std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, search::Song> || std::is_same_v<T, search::Video>) return QString::fromStdString(arg.video_id);
             if constexpr (std::is_same_v<T, search::TopResult>) return arg.video_id ? QString::fromStdString(*arg.video_id) : QString();
            return QString();
        }, item);
    case Artists:
         return QVariant::fromValue(std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, search::Song> || std::is_same_v<T, search::Video> || std::is_same_v<T, search::TopResult>) return arg.artists;
            return std::vector<meta::Artist>();
        }, item));
    case RadioPlaylistId:
        return std::visit([&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, search::Artist>) {
                if (arg.radio_id) return QString::fromStdString(*arg.radio_id);
            }
            return QString();
        }, item);
    case ThumbnailUrl:
        return std::visit([&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, search::Mood>) {
                return QString();
            } else {
                if (!arg.thumbnails.empty()) {
                    return QString::fromStdString(arg.thumbnails.front().url);
                }
            }
            return QString();
        }, item);
    case ArtistsDisplayString:
        return QVariant::fromValue(std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, search::Song> || std::is_same_v<T, search::Video> || std::is_same_v<T, search::Album>) {
                return PlaylistUtils::artistsToString(arg.artists);
            } else if constexpr(std::is_same_v<T, search::TopResult>) {
                if (arg.title) return PlaylistUtils::artistsToString(arg.artists);
            }
            return QString();
        }, item));
    }
    return {};
}

QHash<int, QByteArray> HomeShelfModel::roleNames() const {
    return {
        {Title, "title"},
        {TypeRole, "type"},
        {VideoId, "videoId"},
        {Artists, "artists"},
        {ArtistsDisplayString, "artistsDisplayString"},
        {RadioPlaylistId, "radioPlaylistId"},
        {ThumbnailUrl, "thumbnailUrl"}
    };
}

void HomeShelfModel::triggerItem(int row) {
    if (row < 0 || row >= int(m_items.size())) return;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, search::Album>) {
            if (arg.browse_id) {
                Q_EMIT openAlbum(QString::fromStdString(*arg.browse_id));
            }
        } else if constexpr (std::is_same_v<T, search::Artist>) {
            Q_EMIT openArtist(QString::fromStdString(arg.browse_id), QString::fromStdString(arg.radio_id.value_or("")), QString::fromStdString(arg.shuffle_id.value_or("")));
        } else if constexpr (std::is_same_v<T, search::Playlist>) {
            Q_EMIT openPlaylist(QString::fromStdString(arg.browse_id));
        } else if constexpr (std::is_same_v<T, search::Song>) {
            Q_EMIT openSong(QString::fromStdString(arg.video_id));
        } else if constexpr (std::is_same_v<T, search::Video>) {
            Q_EMIT openVideo(QString::fromStdString(arg.video_id), QString::fromStdString(arg.title));
        } else if constexpr (std::is_same_v<T, search::TopResult>) {
             if (arg.video_id) {
                 Q_EMIT openSong(QString::fromStdString(*arg.video_id));
             }
         else if (!arg.artists.empty() && arg.artists.front().id)
            Q_EMIT openArtist(QString::fromStdString(*arg.artists.front().id), {}, {});
        } else if constexpr (std::is_same_v<T, search::Mood>) {
            Q_EMIT openMood(QString::fromStdString(arg.title), QString::fromStdString(arg.params));
        }
    }, m_items.at(row));
}

AbstractShelfModel::AbstractShelfModel(QObject *parent)
    : AbstractYTMusicModel(parent)
{
}

int AbstractShelfModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : int(m_shelves.size());
}

QVariant AbstractShelfModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= int(m_shelves.size())) return {};
    
    switch (role) {
    case Title:
        return QString::fromStdString(m_shelves.at(index.row()).title);
    case ContentModelRole:
        return QVariant::fromValue(static_cast<QObject*>(m_shelfModels.at(index.row()).get()));
    }
    return {};
}

QHash<int, QByteArray> AbstractShelfModel::roleNames() const {
    return {
        {Title, "title"},
        {ContentModelRole, "contentModel"}
    };
}

void AbstractShelfModel::addShelf(const std::vector<search::SearchResultItem> &contents) {
    auto model = std::make_unique<HomeShelfModel>(this);
    model->setItems(contents);

    connect(model.get(), &HomeShelfModel::openAlbum, this, &AbstractShelfModel::openAlbum);
    connect(model.get(), &HomeShelfModel::openArtist, this, &AbstractShelfModel::openArtist);
    connect(model.get(), &HomeShelfModel::openPlaylist, this, &AbstractShelfModel::openPlaylist);
    connect(model.get(), &HomeShelfModel::openSong, this, &AbstractShelfModel::openSong);
    connect(model.get(), &HomeShelfModel::openVideo, this, &AbstractShelfModel::openVideo);
    connect(model.get(), &HomeShelfModel::openMood, this, &AbstractShelfModel::openMood);

    m_shelfModels.push_back(std::move(model));
}
