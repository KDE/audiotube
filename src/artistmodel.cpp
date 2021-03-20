// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "artistmodel.h"

#include <asyncytmusic.h>

ArtistModel::ArtistModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(this, &ArtistModel::channelIdChanged, this, [=] {
        if (m_channelId.isEmpty()) {
            return;
        }

        setLoading(true);

        YTMusicThread::instance()->fetchArtist(m_channelId);
    });
    connect(&YTMusicThread::instance().get(), &AsyncYTMusic::fetchArtistFinished, this, [=](const artist::Artist &artist) {
        setLoading(false);

        beginResetModel();
        m_artist = artist;
        std::sort(m_artist.thumbnails.begin(), m_artist.thumbnails.end());
        endResetModel();

        Q_EMIT titleChanged();
        Q_EMIT thumbnailUrlChanged();
    });
    connect(&YTMusicThread::instance().get(), &AsyncYTMusic::errorOccurred, this, [=] {
        setLoading(false);
    });
}

template <typename T>
int countItems(const std::optional<artist::Artist::Section<T>> &section) {
    if (!section.has_value()) {
        return 0;
    }

    return section.value().results.size();
};

int ArtistModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : countItems(m_artist.songs)
            + countItems(m_artist.albums)
            + countItems(m_artist.singles)
            + countItems(m_artist.videos);
}

QVariant ArtistModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Title:
        if (index.row() >= countItems(m_artist.songs)
                + countItems(m_artist.albums)
                + countItems(m_artist.singles)) {

            int videoIndex = index.row() - (countItems(m_artist.songs)
                    + countItems(m_artist.albums)
                    + countItems(m_artist.singles));

            if (m_artist.videos.has_value()) {
                return QString::fromStdString(m_artist.videos.value().results[videoIndex].title);
            }
            break;
        } else if (index.row() >= countItems(m_artist.songs) + countItems(m_artist.albums)) {
            int singleIndex = index.row() - (countItems(m_artist.songs) + countItems(m_artist.albums));

            if (m_artist.singles.has_value()) {
               return QString::fromStdString(m_artist.singles.value().results[singleIndex].title);
            }
            break;
        } else if (index.row() >= countItems(m_artist.songs)) {
            int albumIndex = index.row() - countItems(m_artist.songs);

            if (m_artist.albums.has_value()) {
                return QString::fromStdString(m_artist.albums.value().results[albumIndex].title);
            }
            break;
        } else {
            return QString::fromStdString(m_artist.songs.value().results[index.row()].title);
        }
    case Type:
        if (index.row() >= countItems(m_artist.songs)
                + countItems(m_artist.albums)
                + countItems(m_artist.singles)) {

            if (m_artist.videos.has_value()) {
                return Video;
            }
            break;
        } else if (index.row() >= countItems(m_artist.songs) + countItems(m_artist.albums)) {
            if (m_artist.singles.has_value()) {
               return Single;
            }
            break;
        } else if (index.row() >= countItems(m_artist.songs)) {
            if (m_artist.albums.has_value()) {
                return Album;
            }
            break;
        } else {
            return Song;
        }
    case Artists:
        return QVariant::fromValue(std::vector<meta::Artist> {
            {
                m_artist.name,
                m_artist.channel_id
            }
        });
    case VideoId:
        if (index.row() >= countItems(m_artist.songs)
                + countItems(m_artist.albums)
                + countItems(m_artist.singles)) {

            int videoIndex = index.row() - (countItems(m_artist.songs)
                    + countItems(m_artist.albums)
                    + countItems(m_artist.singles));

            if (m_artist.videos.has_value()) {
                return QString::fromStdString(m_artist.videos.value().results[videoIndex].video_id);
            }
            break;
        } else if (index.row() >= countItems(m_artist.songs) + countItems(m_artist.albums)) {
            return QVariant();
        } else if (index.row() >= countItems(m_artist.songs)) {
            return QVariant();
        } else {
            return QString::fromStdString(m_artist.songs.value().results[index.row()].video_id);
        }
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

bool ArtistModel::loading() const
{
    return m_loading;
}

void ArtistModel::setLoading(bool loading)
{
    m_loading = loading;
    Q_EMIT loadingChanged();
}

void ArtistModel::triggerItem(int row)
{
    if (row >= countItems(m_artist.songs)
            + countItems(m_artist.albums)
            + countItems(m_artist.singles)) {

        int videoIndex = row - (countItems(m_artist.songs)
                + countItems(m_artist.albums)
                + countItems(m_artist.singles));

        if (m_artist.videos.has_value()) {
            Q_EMIT openVideo(QString::fromStdString(m_artist.videos.value().results[videoIndex].video_id));
        }
    } else if (row >= countItems(m_artist.songs) + countItems(m_artist.albums)) {
        int singleIndex = row - (countItems(m_artist.songs) + countItems(m_artist.albums));

        if (m_artist.singles.has_value()) {
           Q_EMIT openAlbum(QString::fromStdString(m_artist.singles.value().results[singleIndex].browse_id));
        }
    } else if (row >= countItems(m_artist.songs)) {
        int albumIndex = row - countItems(m_artist.songs);

        if (m_artist.albums.has_value()) {
            Q_EMIT openAlbum(QString::fromStdString(m_artist.albums.value().results[albumIndex].browseId));
        }
    } else {
        Q_EMIT openSong(QString::fromStdString(m_artist.songs.value().results[row].video_id));
    }
}
