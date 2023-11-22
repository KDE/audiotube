// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "localplaylistsmodel.h"
#include "playlistutils.h"

#include "library.h"
#include <qglobal.h>
#include <qsqldatabase.h>
#include <threadeddatabase.h>

#include <KLocalizedString>

Q_DECLARE_METATYPE(std::vector<QString>);

LocalPlaylistsModel::LocalPlaylistsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    importer = new PlaylistImporter(this);
    connect(importer, &PlaylistImporter::playlistEntriesChanged, this, &LocalPlaylistsModel::playlistEntriesChanged);
    connect(importer,&PlaylistImporter::importFinished, this, &LocalPlaylistsModel::importFinished);
    connect(importer, &PlaylistImporter::refreshModel, this, &LocalPlaylistsModel::refreshModel);
    connect(&Library::instance(), &Library::playlistsChanged,
            this, &LocalPlaylistsModel::refreshModel);
    refreshModel();
}

int LocalPlaylistsModel::rowCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : m_playlists.size();
}

QHash<int, QByteArray> LocalPlaylistsModel::roleNames() const
{
    return {
        {Roles::PlaylistId, "playlistId"},
        {Roles::Title, "title"},
        {Roles::Description, "description"},
        {Roles::CreatedOn, "createdOn"},
        {Roles::ThumbnailIds, "thumbnailIds"},
    };
}

QVariant LocalPlaylistsModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Roles::PlaylistId:
        return m_playlists[index.row()].playlistId;
    case Roles::Title:
        return m_playlists[index.row()].title;
    case Roles::Description:
        return m_playlists[index.row()].description;
    case Roles::CreatedOn:
        return m_playlists[index.row()].createdOn;
    case Roles::ThumbnailIds:
        return QVariant::fromValue(m_thumbnailIds.at(index.row()));
    }

    Q_UNREACHABLE();
}

void LocalPlaylistsModel::refreshModel()
{
    QCoro::connect(Library::instance().database().getResults<Playlist>("select * from playlists"), this, [this](const auto &&playlists) {
        m_thumbnailIds.resize(playlists.size());
        for (size_t i = 0; i < playlists.size(); i++) {
            auto future = Library::instance().database().getResults<SingleValue<QString>>("select video_id from playlist_entries where playlist_id = ? order by random() limit 4", playlists.at(i).playlistId);
            QCoro::connect(std::move(future), this, [this, playlists, i](auto &&ids) {
                std::ranges::transform(ids, std::back_inserter(m_thumbnailIds[i]), [](auto &&id) { return id.value; });
                dataChanged(index(i), index(i), {Roles::ThumbnailIds});
            });
        }

        beginResetModel();
        m_playlists = playlists;
        endResetModel();
    });
}
void LocalPlaylistsModel::addPlaylist(const QString &title, const QString &description)
{
    QCoro::connect(Library::instance().database().execute("insert into playlists (title, description) values (?, ?)", title, description), &Library::instance(), &Library::playlistsChanged);
}

void LocalPlaylistsModel::addPlaylistEntry(qint64 playlistId, const QString &videoId, const QString &title, const QString &artist, const QString &album)
{
    importer->addPlaylistEntry(playlistId, videoId, title, artist, album);
}

void LocalPlaylistsModel::addPlaylistEntry(qint64 playlistId, const playlist::Track &track)
{
    importer->addPlaylistEntry(playlistId, track);
}

void LocalPlaylistsModel::importPlaylist(const QString &url)
{
    importer->importPlaylist(url);
}

void LocalPlaylistsModel::renamePlaylist(qint64 playlistId, const QString &name, const QString &description)
{
    importer->renamePlaylist(playlistId, name, description);
}

void LocalPlaylistsModel::deletePlaylist(qint64 playlistId)
{
    QCoro::connect(Library::instance().database().execute("delete from playlists where playlist_id = ?", playlistId), this, &LocalPlaylistsModel::refreshModel);
}

QStringView LocalPlaylistsModel::cropURL(QStringView srcUrl)
{
    // Find entry point
    constexpr auto urlFragment = QStringView(u"?list=");
    qsizetype urlPos = srcUrl.indexOf(urlFragment);
    if (urlPos != -1) {
        urlPos += urlFragment.size();
    } else {
        urlPos = 0;
    }
    auto mid = srcUrl.mid(urlPos);

    // Find exit point
    urlPos = std::min(mid.indexOf(u"?"), mid.indexOf(u"&"));
    return mid.mid(0, urlPos);
}
