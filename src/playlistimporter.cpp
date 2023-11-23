// SPDX-FileCopyrightText: 2023 Théophile Gilgien <theophile@gilgien.net>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "playlistimporter.h"
#include "playlistutils.h"

#include "library.h"
#include <qglobal.h>
#include <qsqldatabase.h>
#include <threadeddatabase.h>

#include <KLocalizedString>

PlaylistImporter::PlaylistImporter(QObject* parent)
    :QObject(parent)
{}


void PlaylistImporter::importPlaylist(const QString &url)
{
    const QString croppedURL = this->cropURL(url).toString(), title = i18n("Unknown"), description = i18n("No description");
    QCoro::connect(Library::instance().database().execute("insert into playlists (title, description) values (?, ?)", title, description), &Library::instance(), [this, croppedURL]() {
        QCoro::connect(Library::instance().database().getResults<SingleValue<qint64>>("select * from playlists"), &Library::instance(), [this, croppedURL](auto &&playlists) {
            const quint64 playlistId = playlists.back().value;
            Q_EMIT Library::instance().playlistsChanged();

            QCoro::connect(YTMusicThread::instance()->fetchPlaylist(croppedURL), this, [this, playlistId](auto &&playlist) {
                this->renamePlaylist(playlistId, QString::fromStdString(playlist.title), QString::fromStdString(playlist.author.name));

                for (const auto& track : playlist.tracks) {
                    if (track.is_available && track.video_id) {
                        this->addPlaylistEntry(playlistId, track);
                    }
                }

                Q_EMIT Library::instance().playlistsChanged();
                Q_EMIT importFinished();
            });
        });
    });
}

void PlaylistImporter::addPlaylistEntry(qint64 playlistId, const QString &videoId, const QString &title, const QString &artist, const QString &album)
{
    QCoro::connect(Library::instance().addSong(videoId, title, artist, album), this, [=, this] {
        QCoro::connect(Library::instance().database().execute("insert into playlist_entries (playlist_id, video_id) values (?, ?)", playlistId, videoId), this, [=, this] {
            Q_EMIT playlistEntriesChanged(playlistId);
        });
    });
}

void PlaylistImporter::addPlaylistEntry(qint64 playlistId, const playlist::Track &track)
{
    const QString videoId = track.video_id.value().c_str();
    const QString title   = (!track.title.empty()) ? QString::fromStdString(track.title) : i18n("No title");
    const QString artists = PlaylistUtils::artistsToString(track.artists);
    const QString album   = (track.album ) ? QString::fromStdString(track.album->name) : i18n("No album");
    this->addPlaylistEntry(playlistId, videoId, title, artists, album);
}

void PlaylistImporter::renamePlaylist(qint64 playlistId, const QString &name, const QString &description)
{
    QCoro::connect(Library::instance().database().execute("update playlists set title = ? , description = ? where playlist_id = ?", name, description, playlistId), this, &PlaylistImporter::refreshModel);
}


QStringView PlaylistImporter::cropURL(QStringView srcUrl)
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
