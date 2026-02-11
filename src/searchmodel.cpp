// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "searchmodel.h"
#include "playlistutils.h"

#include <ranges>

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
        QCoro::connect(std::move(future), this, [=, this](const std::vector<search::SearchResultItem> &&results) {
            beginResetModel();
            setLoading(false);
            m_searchResults = std::move(results);

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
            } else if constexpr (std::is_same_v<T, search::TopResult>) {
                if (arg.title) {
                    return *arg.title;
                } else {
                    if (!arg.artists.empty()) {
                        return arg.artists.front().name;
                    } else {
                        return std::string();
                    }
                }
            } else if constexpr (std::is_same_v<T, search::Mood>) {
                return arg.title;
            } else {
                Q_UNREACHABLE();
            }
        }, m_searchResults.at(index.row())));
    case TypeRole:
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
            } else if constexpr (std::is_same_v<T, search::TopResult>) {
                return Type::TopResult;
            } else if constexpr (std::is_same_v<T, search::Mood>) {
                return Type::Mood;
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
            } else if constexpr (std::is_same_v<T, search::TopResult>) {
                if (arg.video_id) {
                    return QString::fromStdString(*arg.video_id);
                } else {
                    return QString();
                }
            } else if constexpr (std::is_same_v<T, search::Mood>) {
                return QString();
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
            } else if constexpr (std::is_same_v<T, search::TopResult>) {
                return arg.artists;
            } else if constexpr (std::is_same_v<T, search::Mood>) {
                return std::vector<meta::Artist>();
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
    case ThumbnailUrl:
        return std::visit([&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, search::Mood>) {
                return QString();
            } else {
                if (!arg.thumbnails.empty()) {
                    return QString::fromStdString(arg.thumbnails.front().url);
                }
                return QString();
            }

        }, m_searchResults.at(index.row()));
    case ArtistsDisplayString:
        return QVariant::fromValue(std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, search::Song> || std::is_same_v<T, search::Video> || std::is_same_v<T, search::Album>) {
                return PlaylistUtils::artistsToString(arg.artists);
            } else if constexpr(std::is_same_v<T, search::TopResult>) {
                if (arg.title) {
                    return PlaylistUtils::artistsToString(arg.artists);
                }
            }
            return QString();
        }, m_searchResults.at(index.row())));
    }

    Q_UNREACHABLE();

    return {};
}

QHash<int, QByteArray> SearchModel::roleNames() const
{
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
            } else {
                if (!arg.artists.empty() && arg.artists.front().id) {
                    Q_EMIT openArtist(QString::fromStdString(*arg.artists.front().id), {}, {});
                }
            }
        } else if constexpr (std::is_same_v<T, search::Mood>) {
            // TODO: Implement mood opening
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
        } else if constexpr(std::is_same_v<T, search::TopResult>) {
            return SearchModel::Type::TopResult;
        } else if constexpr(std::is_same_v<T, search::Mood>) {
            return SearchModel::Type::Mood;
        } else {
            Q_UNREACHABLE();
        }
    }, item);
}
