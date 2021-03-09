#include "albummodel.h"

#include <QDebug>

AlbumModel::AlbumModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(this, &AlbumModel::browseIdChanged, this, [=] {
        setLoading(true);
        AsyncYTMusic::instance().fetchAlbum(m_browseId);
    });
    connect(&AsyncYTMusic::instance(), &AsyncYTMusic::fetchAlbumFinished, this, [=](const album::Album &album) {
        setLoading(false);

        beginResetModel();
        m_album = album;
        endResetModel();
        Q_EMIT titleChanged();
    });
}

int AlbumModel::rowCount(const QModelIndex &parent) const
{
    return m_album.tracks.size();
}

QVariant AlbumModel::data(const QModelIndex &index, int role) const
{
    return QString::fromStdString(m_album.tracks.at(index.row()).title);
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
