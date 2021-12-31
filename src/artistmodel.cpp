// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "artistmodel.h"

#include <asyncytmusic.h>

ArtistModel::ArtistModel(QObject *parent)
    : AbstractYTMusicModel(parent)
    , m_view(albums, singles, songs, videos)
{
    connect(this, &ArtistModel::channelIdChanged, this, [this] {
        if (m_channelId.isEmpty()) {
            return;
        }

        setLoading(true);

        auto future = YTMusicThread::instance()->fetchArtist(m_channelId);
        connectFuture(future, this, [=, this](const artist::Artist &artist) {
            setLoading(false);

            beginResetModel();
            m_artist = artist;
            std::sort(m_artist.thumbnails.begin(), m_artist.thumbnails.end());

            albums = m_artist.albums ? m_artist.albums->results : std::vector<artist::Artist::Album>();
            singles = m_artist.singles ? m_artist.singles->results : std::vector<artist::Artist::Single>();
            songs = m_artist.songs ? m_artist.songs->results : std::vector<artist::Artist::Song>();
            videos = m_artist.videos ? m_artist.videos->results : std::vector<artist::Artist::Video>();

            // std::span can't know if the data pointer underneath it was changed, so re-create
            m_view = MultiIterableView(albums, singles, songs, videos);
            endResetModel();

            Q_EMIT titleChanged();
            Q_EMIT thumbnailUrlChanged();
        });
    });
    connect(&YTMusicThread::instance().get(), &AsyncYTMusic::errorOccurred, this, [this] {
        setLoading(false);
    });
}

int ArtistModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_view.size();
}

QVariant ArtistModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Title:
        return QString::fromStdString(std::visit([&](auto&& item) {
            using T = std::decay_t<decltype(item)>;
            if constexpr(std::is_same_v<T, artist::Artist::Album>) {
                return item.title;
            } else if constexpr(std::is_same_v<T, artist::Artist::Single>) {
                return item.title;
            } else if constexpr(std::is_same_v<T, artist::Artist::Song>) {
                return item.title;
            } else if constexpr(std::is_same_v<T, artist::Artist::Video>) {
                return item.title;
            }

            Q_UNREACHABLE();
        }, m_view[index.row()]));
    case Type:
        return std::visit([&](auto&& item) {
            using T = std::decay_t<decltype(item)>;
            if constexpr(std::is_same_v<T, artist::Artist::Album>) {
                return Type::Album;
            } else if constexpr(std::is_same_v<T, artist::Artist::Single>) {
                return Type::Single;
            } else if constexpr(std::is_same_v<T, artist::Artist::Song>) {
                return Type::Song;
            } else if constexpr(std::is_same_v<T, artist::Artist::Video>) {
                return Type::Video;
            }

            Q_UNREACHABLE();
        }, m_view[index.row()]);
    case Artists:
        return QVariant::fromValue(std::vector<meta::Artist> {
            {
                m_artist.name,
                m_artist.channel_id
            }
        });
    case VideoId:
        return std::visit([&](auto&& item) {
            using T = std::decay_t<decltype(item)>;
            if constexpr(std::is_same_v<T, artist::Artist::Album>) {
                return QVariant();
            } else if constexpr(std::is_same_v<T, artist::Artist::Single>) {
                return QVariant();
            } else if constexpr(std::is_same_v<T, artist::Artist::Song>) {
                return QVariant(QString::fromStdString(item.video_id));
            } else if constexpr(std::is_same_v<T, artist::Artist::Video>) {
                return QVariant(QString::fromStdString(item.video_id));
            }

            Q_UNREACHABLE();
        }, m_view[index.row()]);
    }

    Q_UNREACHABLE();

    return {};
}

QHash<int, QByteArray> ArtistModel::roleNames() const
{
    return {
        {Title, "title"},
        {Type, "type"},
        {Artists, "artists"},
        {VideoId, "videoId"}
    };
}

QString ArtistModel::channelId() const
{
    return m_channelId;
}

void ArtistModel::setChannelId(const QString &channelId)
{
    m_channelId = channelId;
    Q_EMIT channelIdChanged();
}

QString ArtistModel::title() const
{
    return QString::fromStdString(m_artist.name);
}

QUrl ArtistModel::thumbnailUrl() const
{
    if (m_artist.thumbnails.empty()) {
        return QUrl();
    }

    return QUrl(QString::fromStdString(m_artist.thumbnails.back().url));
}

void ArtistModel::triggerItem(int row)
{
    std::visit([&](auto&& item) {
        using T = std::decay_t<decltype(item)>;
        if constexpr(std::is_same_v<T, artist::Artist::Album>) {
            Q_EMIT openAlbum(QString::fromStdString(item.browse_id));
        } else if constexpr(std::is_same_v<T, artist::Artist::Single>) {
            Q_EMIT openAlbum(QString::fromStdString(item.browse_id));
        } else if constexpr(std::is_same_v<T, artist::Artist::Song>) {
            Q_EMIT openSong(QString::fromStdString(item.video_id));
        } else if constexpr(std::is_same_v<T, artist::Artist::Video>) {
            Q_EMIT openVideo(QString::fromStdString(item.video_id), QString::fromStdString(item.title));
        } else {
            Q_UNREACHABLE();
        }
    }, m_view[row]);
}
