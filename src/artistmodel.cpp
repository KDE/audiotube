#include "artistmodel.h"

#include <asyncytmusic.h>

#include <QDebug>

ArtistModel::ArtistModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(this, &ArtistModel::channelIdChanged, this, [=] {
        if (m_channelId.isEmpty()) {
            return;
        }

        setLoading(true);

        AsyncYTMusic::instance().fetchArtist(m_channelId);
    });
    connect(&AsyncYTMusic::instance(), &AsyncYTMusic::fetchArtistFinished, this, [=](const artist::Artist &artist) {
        setLoading(false);

        beginResetModel();
        m_artist = artist;
        endResetModel();

        Q_EMIT titleChanged();
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
    }

    Q_UNREACHABLE();

    return {};
}

QHash<int, QByteArray> ArtistModel::roleNames() const
{
    return {
        {Title, "title"},
        {Type, "type"}
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
