// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "albummodel.h"

#include <QDebug>

AlbumModel::AlbumModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(this, &AlbumModel::browseIdChanged, this, [=] {
        setLoading(true);
        YTMusicThread::instance()->fetchAlbum(m_browseId);
    });
    connect(&YTMusicThread::instance().get(), &AsyncYTMusic::fetchAlbumFinished, this, [=](const album::Album &album) {
        setLoading(false);

        beginResetModel();
        m_album = album;
        endResetModel();
        std::sort(m_album.thumbnails.begin(), m_album.thumbnails.end());

        Q_EMIT titleChanged();
        Q_EMIT thumbnailUrlChanged();
        Q_EMIT playlistIdChanged();
    });
    connect(&YTMusicThread::instance().get(), &AsyncYTMusic::errorOccurred, this, [=] {
        setLoading(false);
    });
}

int AlbumModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : int(m_album.tracks.size());
}

QVariant AlbumModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Title:
        return QString::fromStdString(m_album.tracks[index.row()].title);
    case VideoId:
        return QString::fromStdString(m_album.tracks[index.row()].video_id.value_or(std::string()));
    case Artists:
        return QVariant::fromValue(std::vector<meta::Artist> {
            {
                m_album.tracks[index.row()].artists,
                std::nullopt
            }
        });
    }

    Q_UNREACHABLE();

    return {};
}

QHash<int, QByteArray> AlbumModel::roleNames() const
{
    return {
        {Title, "title"},
        {VideoId, "videoId"},
        {Artists, "artists"}
    };
}

QString AlbumModel::browseId() const
{
    return m_browseId;
}

void AlbumModel::setBrowseId(const QString &value)
{
    m_browseId = value;
    Q_EMIT browseIdChanged();
}

QString AlbumModel::title() const
{
    return QString::fromStdString(m_album.title);
}

bool AlbumModel::loading() const
{
    return m_loading;
}

void AlbumModel::setLoading(bool loading)
{
    m_loading = loading;
    Q_EMIT loadingChanged();
}


QUrl AlbumModel::thumbnailUrl() const
{
    if (m_album.thumbnails.empty()) {
        return QUrl();
    }

    return QUrl(QString::fromStdString(m_album.thumbnails.back().url));
}

QString AlbumModel::playlistId() const
{
    return QString::fromStdString(m_album.playlist_id);
}
