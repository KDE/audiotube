// SPDX-FileCopyrightText: 2022 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import org.kde.ytmusic 1.0

Controls.Menu{
    id: menu
    property string videoId
    property string songTitle
    property var artists
    property string artistsDisplayString


    function openForSong(videoId, songTitle, artists, artistsDisplayString) {
        menu.videoId = videoId
        menu.songTitle = songTitle
        menu.artists = artists
        menu.artistsDisplayString = artistsDisplayString
        menu.popup()
    }

    Controls.MenuItem {
        text: i18n("Play Next")
        icon.name: "go-next"
        onTriggered: UserPlaylistModel.playNext(menu.videoId, menu.SongTitle, artists)
    }

    Controls.MenuItem {
        text: i18n("Add to queue")
        icon.name: "media-playlist-append"
        onTriggered: UserPlaylistModel.append(menu.videoId, menu.SongTitle, artists)
    }

    Controls.MenuItem {
        readonly property QtObject favouriteWatcher: Library.favouriteWatcher(menu.videoId)
        text: favouriteWatcher ? (favouriteWatcher.isFavourite ? i18n("Remove Favourite"): i18n("Add Favourite")): ""
        icon.name: favouriteWatcher ? (favouriteWatcher.isFavourite ? "starred-symbolic" : "non-starred-symbolic") : ""
        onTriggered: {
            if (favouriteWatcher) {
                if (favouriteWatcher.isFavourite) {
                    Library.removeFavourite(menu.videoId)
                } else {
                    Library.addFavourite(videoId, songTitle, artistsDisplayString, "")
                }
            }
        }
    }
}
