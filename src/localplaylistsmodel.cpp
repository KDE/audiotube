// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "localplaylistsmodel.h"
#include "playlistutils.h"

#include "library.h"
#include <qfuture.h>
#include <qglobal.h>
#include <qsqldatabase.h>
#include <threadeddatabase.h>

#include <KLocalizedString>

Q_DECLARE_METATYPE(std::vector<QString>);

LocalPlaylistsModel::LocalPlaylistsModel(QObject *parent)
    : QAbstractListModel(parent)
{
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
    QCoro::connect(Library::instance().addSong(videoId, title, artist, album), this, [=, this] {
        QCoro::connect(Library::instance().database().execute("insert into playlist_entries (playlist_id, video_id) values (?, ?)", playlistId, videoId), this, [=, this] {
            Q_EMIT playlistEntriesChanged(playlistId);
        });
    });
}

void LocalPlaylistsModel::addPlaylistEntry(qint64 playlistId, const playlist::Track &track)
{
    const QString videoId = track.video_id.value().c_str();
    const QString title   = (!track.title.empty()) ? QString::fromStdString(track.title) : i18n("No title");
    const QString artists = PlaylistUtils::artistsToString(track.artists);
    const QString album   = (track.album ) ? QString::fromStdString(track.album->name) : i18n("No album");
    this->addPlaylistEntry(playlistId, videoId, title, artists, album);
}

void LocalPlaylistsModel::importPlaylist(const QString &url)
{
    const QString croppedURL = this->cropURL(url).toString(), title = i18n("Unknown"), description = i18n("No description");
    QCoro::connect(Library::instance().database().execute("insert into playlists (title, description) values (?, ?)", title, description), &Library::instance(), [this, croppedURL]() {
        QCoro::connect(Library::instance().database().getResults<SingleValue<qint64>>("select * from playlists"), &Library::instance(), [this, croppedURL](const auto& playlists) {
            const quint64 playlistId = playlists.back().value;
            Q_EMIT Library::instance().playlistsChanged();

            QCoro::connect(YTMusicThread::instance()->fetchPlaylist(croppedURL), this, [this, playlistId](const auto& playlist) {
                this->renamePlaylist(playlistId, QString::fromStdString(playlist.title), QString::fromStdString(playlist.author.name));

                for (const auto& track : playlist.tracks) {
                    if (track.is_available && track.video_id) {
                        this->addPlaylistEntry(playlistId, track);
                    }
                }

                Q_EMIT Library::instance().playlistsChanged();
            });
        });
    });
}

void LocalPlaylistsModel::renamePlaylist(qint64 playlistId, const QString &name, const QString &description)
{
    QCoro::connect(Library::instance().database().execute("update playlists set title = ? , description = ? where playlist_id = ?", name, description, playlistId), this, &LocalPlaylistsModel::refreshModel);
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
