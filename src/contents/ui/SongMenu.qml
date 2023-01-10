// SPDX-FileCopyrightText: 2022 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import org.kde.ytmusic 1.0
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Layouts 1.15


Item {
    function openForSong(videoId, songTitle, artists, artistsDisplayString) {
        menu.videoId = videoId
        drawer.videoId = videoId

        menu.songTitle = songTitle
        drawer.songTitle = songTitle

        menu.artists = artists
        drawer.artists = artists

        menu.artistsDisplayString = artistsDisplayString
        drawer.artistsDisplayString = artistsDisplayString

        if (Kirigami.Settings.isMobile) {
            drawer.interactive = true
            drawer.open()
         } else {
            menu.popup()
        }
    }
    Controls.Drawer {
        edge: Qt.BottomEdge
        height:contents.implicitHeight+20
        width: applicationWindow().width
        interactive: false

        onClosed: drawer.interactive = false

        id: drawer
        property string videoId
        property string songTitle
        property var artists
        property string artistsDisplayString
        ColumnLayout {
            id: contents
            anchors.fill: parent

            Kirigami.Icon {
                Layout.margins: Kirigami.Units.smallSpacing
                source: "arrow-down"
                implicitWidth: Kirigami.Units.gridUnit
                implicitHeight: Kirigami.Units.gridUnit
                Layout.alignment: Qt.AlignHCenter
            }
            Kirigami.BasicListItem{

                label: i18n("Play Next")
                icon: "go-next"
                onClicked: {
                    UserPlaylistModel.playNext(drawer.videoId, drawer.songTitle, drawer.artists)
                    drawer.close()
                }
            }
            Kirigami.BasicListItem{

                label: i18n("Add to queue")
                icon: "media-playlist-append"
                onClicked: {
                    UserPlaylistModel.append(drawer.videoId, drawer.songTitle, drawer.artists)
                    drawer.close()
                }
            }
            Kirigami.BasicListItem{
                readonly property QtObject favouriteWatcher: Library.favouriteWatcher(drawer.videoId)

                label: favouriteWatcher ? (favouriteWatcher.isFavourite ? i18n("Remove Favourite"): i18n("Add Favourite")): ""
                icon: favouriteWatcher ? (favouriteWatcher.isFavourite ? "starred-symbolic" : "non-starred-symbolic") : ""
                onClicked: {
                    if (favouriteWatcher) {
                        if (favouriteWatcher.isFavourite) {
                            Library.removeFavourite(drawer.videoId)
                        } else {
                            Library.addFavourite(drawer.videoId, drawer.songTitle, drawer.artistsDisplayString, "")
                        }
                    }
                    drawer.close()
                }
            }
            Item{
                Layout.fillHeight: true
            }

        }
    }

    Controls.Menu {
        property string videoId
        property string songTitle
        property var artists
        property string artistsDisplayString
        id: menu

        Controls.MenuItem {
            text: i18n("Play Next")
            icon.name: "go-next"
            onTriggered: UserPlaylistModel.playNext(menu.videoId, menu.songTitle, menu.artists)
        }

        Controls.MenuItem {
            text: i18n("Add to queue")
            icon.name: "media-playlist-append"
            onTriggered: UserPlaylistModel.append(menu.videoId, menu.songTitle, menu.artists)
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
                        Library.addFavourite(menu.videoId, menu.songTitle, menu.artistsDisplayString, "")
                    }
                }
            }
        }
    }
}
