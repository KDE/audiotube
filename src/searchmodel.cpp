// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "searchmodel.h"

SearchModel::SearchModel(QObject *parent)
    : AbstractYTMusicModel(parent)
{
    connect(this, &SearchModel::searchQueryChanged, this, [this] {
        if (m_searchQuery.isEmpty()) {
            beginResetModel();
            m_searchResults.clear();
            endResetModel();
            return;
        }

        setLoading(true);
        auto future = YTMusicThread::instance()->search(m_searchQuery);
        connectFuture(future, this, [=, this](const std::vector<search::SearchResultItem> &results) {
            beginResetModel();
            setLoading(false);
            m_searchResults = results;
            endResetModel();
        });
    });
    connect(&YTMusicThread::instance().get(), &AsyncYTMusic::errorOccurred, this, [this] {
        setLoading(false);
    });
}

int SearchModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : int(m_searchResults.size());
}

QVariant SearchModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Title:
        return QString::fromStdString(std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, search::Album>) {
                return arg.title;
            } else if constexpr (std::is_same_v<T, search::Artist>) {
                return arg.artist;
            } else if constexpr (std::is_same_v<T, search::Playlist>) {
                return arg.title;
            } else if constexpr (std::is_same_v<T, search::Song>) {
                return arg.title;
            } else if constexpr (std::is_same_v<T, search::Video>) {
                return arg.title;
            } else {
                Q_UNREACHABLE();
            }
        }, m_searchResults.at(index.row())));
    case Type:
        return std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, search::Album>) {
                return Type::Album;
            } else if constexpr (std::is_same_v<T, search::Artist>) {
                return Type::Artist;
            } else if constexpr (std::is_same_v<T, search::Playlist>) {
                return Type::Playlist;
            } else if constexpr (std::is_same_v<T, search::Song>) {
                return Type::Song;
            } else if constexpr (std::is_same_v<T, search::Video>) {
                return Type::Video;
            } else {
                Q_UNREACHABLE();
            }
        }, m_searchResults.at(index.row()));
    case VideoId:
        return std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, search::Album>) {
                return QString();
            } else if constexpr (std::is_same_v<T, search::Artist>) {
                return QString();
            } else if constexpr (std::is_same_v<T, search::Playlist>) {
                return QString();
            } else if constexpr (std::is_same_v<T, search::Song>) {
                return QString::fromStdString(arg.video_id);
            } else if constexpr (std::is_same_v<T, search::Video>) {
                return QString::fromStdString(arg.video_id);
            } else {
                Q_UNREACHABLE();
            }
        }, m_searchResults.at(index.row()));
    case Artists:
        return QVariant::fromValue(std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, search::Album>) {
                return std::vector<meta::Artist>();
            } else if constexpr (std::is_same_v<T, search::Artist>) {
                return std::vector<meta::Artist>();
            } else if constexpr (std::is_same_v<T, search::Playlist>) {
                return std::vector<meta::Artist>();
            } else if constexpr (std::is_same_v<T, search::Song>) {
                return arg.artists;
            } else if constexpr (std::is_same_v<T, search::Video>) {
                return arg.artists;
            } else {
                Q_UNREACHABLE();
            }
        }, m_searchResults.at(index.row())));
    case RadioPlaylistId:
        return std::visit([&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, search::Artist>) {
                if (arg.radio_id) {
                    return QString::fromStdString(*arg.radio_id);
                }
            }

            return QString();
        }, m_searchResults.at(index.row()));
    }

    Q_UNREACHABLE();

    return {};
}

QHash<int, QByteArray> SearchModel::roleNames() const
{
    return {
        {Title, "title"},
        {Type, "type"},
        {VideoId, "videoId"},
        {Artists, "artists"},
        {RadioPlaylistId, "radioPlaylistId"},
    };
}

QString SearchModel::searchQuery() const
{
    return m_searchQuery;
}

void SearchModel::setSearchQuery(const QString &searchQuery)
{
    m_searchQuery = searchQuery;
    Q_EMIT searchQueryChanged();
}

void SearchModel::triggerItem(int row)
{
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, search::Album>) {
            Q_EMIT openAlbum(QString::fromStdString(arg.browse_id));
        } else if constexpr (std::is_same_v<T, search::Artist>) {
            Q_EMIT openArtist(QString::fromStdString(arg.browse_id));
        } else if constexpr (std::is_same_v<T, search::Playlist>) {
            Q_EMIT openPlaylist(QString::fromStdString(arg.browse_id));
        } else if constexpr (std::is_same_v<T, search::Song>) {
            Q_EMIT openSong(QString::fromStdString(arg.video_id));
        } else if constexpr (std::is_same_v<T, search::Video>) {
            Q_EMIT openVideo(QString::fromStdString(arg.video_id), QString::fromStdString(arg.title));
        } else {
            Q_UNREACHABLE();
        }
    }, m_searchResults.at(row));
}


int SearchModel::itemType(search::SearchResultItem const &item)
{
    return std::visit([&](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr(std::is_same_v<T, search::Album>) {
            return SearchModel::Type::Album;
        } else if constexpr(std::is_same_v<T, search::Artist>) {
            return SearchModel::Type::Artist;
        } else if constexpr(std::is_same_v<T, search::Playlist>) {
            return SearchModel::Type::Playlist;
        } else if constexpr(std::is_same_v<T, search::Song>) {
            return SearchModel::Type::Song;
        } else if constexpr(std::is_same_v<T, search::Video>) {
            return SearchModel::Type::Video;
        } else {
            Q_UNREACHABLE();
        }
    }, item);
}
