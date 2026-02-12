// SPDX-FileCopyrightText: 2026 Tanveer Ahmed Mansuri
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "exploremodel.h"
#include "playlistutils.h"
#include "asyncytmusic.h"
#include <QCoroFuture>

// --- ExploreModel ---

ExploreModel::ExploreModel(QObject *parent)
	: AbstractShelfModel(parent)
{
    refresh();
}

void ExploreModel::refresh() {
    setLoading(true);
    
    // Fetch charts first
    auto future = YTMusicThread::instance()->fetchCharts();
    QCoro::connect(std::move(future), this, [this](std::vector<home::Shelf> charts) {
         
         // Then fetch moods
         auto moodFuture = YTMusicThread::instance()->fetchMoodCategories();
         auto chartsPtr = std::make_shared<std::vector<home::Shelf>>(std::move(charts));
         QCoro::connect(std::move(moodFuture), this, [this, chartsPtr](std::vector<home::Shelf> moods) {
             beginResetModel();
             m_shelves = std::move(*chartsPtr);
             
             // Append moods
             std::move(moods.begin(), moods.end(), std::back_inserter(m_shelves));
             
             m_shelfModels.clear();
             for (const auto &shelf : m_shelves) {
                addShelf(shelf.contents);
             }
             endResetModel();
             setLoading(false);
         });
    });
}


// --- MoodPlaylistsModel ---

MoodPlaylistsModel::MoodPlaylistsModel(QObject *parent) : AbstractYTMusicModel(parent) {}

int MoodPlaylistsModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : int(m_items.size());
}

QVariant MoodPlaylistsModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= int(m_items.size())) return {};

    const auto &item = m_items.at(index.row());
    
    // Expecting playlists mostly
    return std::visit([&](auto&& arg) -> QVariant {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, search::Playlist>) {
            switch(role) {
                case Title: return QString::fromStdString(arg.title);
                case TypeRole: return SearchModel::Playlist; // reuse value
                case VideoId: return QString::fromStdString(arg.browse_id); // using videoId role for browseId often
                case ThumbnailUrl: 
                    if (!arg.thumbnails.empty()) return QString::fromStdString(arg.thumbnails.front().url);
                    return QString();
                case ItemCount: return QString::fromStdString(arg.item_count);
                case Author: return arg.author ? QString::fromStdString(*arg.author) : QString();
                case Artists: return QString();
            }
        }
        return {};
    }, item);
}

QHash<int, QByteArray> MoodPlaylistsModel::roleNames() const {
    return {
        {Title, "title"},
        {TypeRole, "type"},
        {VideoId, "videoId"}, // mapped to browseId for playlists
        {ThumbnailUrl, "thumbnailUrl"},
        {ItemCount, "itemCount"},
        {Author, "author"},
        {Artists, "artists"},
    };
}

QString MoodPlaylistsModel::params() const {
    return m_params;
}

void MoodPlaylistsModel::setParams(const QString &params) {
    if (m_params == params) return;
    m_params = params;
    Q_EMIT paramsChanged();
    load();
}

QString MoodPlaylistsModel::title() const {
    return m_title;
}

void MoodPlaylistsModel::setTitle(const QString &title) {
    if (m_title == title) return;
    m_title = title;
    Q_EMIT titleChanged();
}

void MoodPlaylistsModel::load() {
    if (m_params.isEmpty()) return;
    setLoading(true);
    
    auto future = YTMusicThread::instance()->fetchMoodPlaylists(m_params);
    QCoro::connect(std::move(future), this, [this](std::vector<search::SearchResultItem> items) {
        beginResetModel();
        m_items = std::move(items);
        endResetModel();
        setLoading(false);
    });
}

void MoodPlaylistsModel::triggerItem(int row) {
    if (row < 0 || row >= int(m_items.size())) return;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, search::Playlist>) {
            Q_EMIT openPlaylist(QString::fromStdString(arg.browse_id));
        }
    }, m_items.at(row));
}
