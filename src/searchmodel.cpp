// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "searchmodel.h"

#include <QDebug>

SearchModel::SearchModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(this, &SearchModel::searchQueryChanged, this, [=] {
        if (m_searchQuery.isEmpty()) {
            beginResetModel();
            m_searchResults.clear();
            endResetModel();
            return;
        }

        setLoading(true);
        AsyncYTMusic::instance().search(m_searchQuery);
    });
    connect(&AsyncYTMusic::instance(), &AsyncYTMusic::searchFinished, this, [=](const std::vector<search::SearchResultItem> &results) {
        beginResetModel();
        setLoading(false);
        m_searchResults = results;
        endResetModel();
    });
}

SearchModel::~SearchModel() = default;

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
        {Artists, "artists"}
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

bool SearchModel::loading() const
{
    return m_loading;
}

void SearchModel::setLoading(bool loading)
{
    m_loading = loading;
    Q_EMIT loadingChanged();
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
            Q_EMIT openVideo(QString::fromStdString(arg.video_id));
        } else {
            Q_UNREACHABLE();
        }
    }, m_searchResults.at(row));
}
