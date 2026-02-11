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

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    title: i18nc("@title:window", "Home")
    objectName: "homePage"

    readonly property SongMenu songMenu: SongMenu {}

    ColumnLayout {
        spacing: Kirigami.Units.gridUnit * 3

        Repeater {
            model: HomeModel {
                id: homeModel
                onOpenSong: (videoId) => applicationWindow().play(videoId)
                onOpenPlaylist: (playlistId) => applicationWindow().playPlaylist(playlistId)
                onOpenAlbum: (browseId) => {
                    root.Kirigami.PageStack.push(Qt.createComponent('org.kde.audiotube', 'AlbumPage.qml'), {
                        browseId: browseId
                    })
                }
                onOpenArtist: (browseId, radioId, shuffleId) => {
                    root.Kirigami.PageStack.push(Qt.createComponent('org.kde.audiotube', 'ArtistsPage.qml'), {
                        browseId: browseId
                    })
                }
                onOpenVideo: (videoId, title) => {
                     applicationWindow().play(videoId)
                }
            }

            delegate: DelegateChooser {
                id: shelfDelegate

                DelegateChoice {
                    row: 0
                    HeroHomeShelf {
                        songMenu: root.songMenu
                        page: root
                    }
                }

                DelegateChoice {
                    HomeShelf {
                        songMenu: root.songMenu
                    }
                }
            }
        }
    }
}
