// SPDX-FileCopyrightText: 2026 Tanveer Ahmed Mansuri
// SPDX-FileCopyrightText: 2026 Carl Schwan <carlschwan@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.audiotube
import QtQml

Kirigami.ScrollablePage {
    id: root

    SongMenu {
        id: menu
        parentItem: root.Controls.ApplicationWindow.window
    }

    title: i18nc("@title:window", "Explore")
    objectName: "explorePage"

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    
    ColumnLayout {
        spacing: Kirigami.Units.largeSpacing * 7

        Repeater {
            model: ExploreModel {
                id: exploreModel
                onOpenSong: (videoId) => applicationWindow().play(videoId)
                onOpenPlaylist: (playlistId) => applicationWindow().playPlaylist(playlistId)
                onOpenAlbum: (browseId) => {
                    root.Kirigami.PageStack.push(Qt.createComponent('org.kde.audiotube', 'AlbumPage'), {
                        browseId: browseId
                    })
                }
                onOpenArtist: (browseId, radioId, shuffleId) => {
                    root.Kirigami.PageStack.push(Qt.createComponent('org.kde.audiotube', 'ArtistPage'), {
                        browseId: browseId
                    })
                }
                onOpenVideo: (videoId, title) => {
                     applicationWindow().play(videoId)
                }
                onOpenMood: (title, params) => {
                    root.Kirigami.PageStack.push(Qt.createComponent('org.kde.audiotube', 'MoodPage'), {
                        moodTitle: title,
                        moodParams: params
                    })
                }
            }

            delegate: HomeShelf {
                songMenu: menu
            }
        }
    }
}
