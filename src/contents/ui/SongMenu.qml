// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import QtQuick.Controls as Controls
import org.kde.ytmusic 1.0
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Layouts 1.15

import "dialogs"

Item {
    id:root
    property string videoId
    property list<Kirigami.Action> pageSpecificActions

    ShareMenu {
        id: shareMenu
    }
    PlaylistDialog{
        id: playlistDialog
    }
    function openForSong(videoId, songTitle, artists, artistsDisplayString) {
        shareMenu.url = "https://music.youtube.com/watch?v=" + videoId
        shareMenu.inputTitle= songTitle

        menu.videoId = videoId
        drawer.videoId = videoId
        playlistDialog.videoId = videoId
        root.videoId = videoId

        menu.songTitle = songTitle
        drawer.songTitle = songTitle
        playlistDialog.songTitle = songTitle

        menu.artists = artists
        drawer.artists = artists

        menu.artistsDisplayString = artistsDisplayString
        drawer.artistsDisplayString = artistsDisplayString
        playlistDialog.artists = artistsDisplayString


        if (Kirigami.Settings.isMobile) {
            drawer.interactive = true
            drawer.open()
         } else {
            menu.popup()
        }
    }
    BottomDrawer {
        id: drawer
        property string videoId
        property string songTitle
        property var artists
        property string artistsDisplayString

        headerContentItem: RowLayout {
            spacing: 10
            Layout.topMargin: 0
            Kirigami.ShadowedRectangle {
                Layout.rightMargin: 5
                Layout.topMargin: 0
                color: Kirigami.Theme.backgroundColor
                width: 60
                height: width
                radius: 5
                shadow.size: 15
                shadow.xOffset: 5
                shadow.yOffset: 5
                shadow.color: Qt.rgba(0, 0, 0, 0.2)
                RoundedImage {
                    source: thumbnailSource.cachedPath
                    height: width
                    width: parent.width
                    radius: parent.radius
                }
            }
            ColumnLayout {
                Layout.margins: 0
                Layout.topMargin: 0
                Controls.Label{
                    text: drawer.songTitle
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
                Controls.Label{
                    text: drawer.artistsDisplayString
                    color: Kirigami.Theme.disabledTextColor
                    elide: Text.ElideRight
                    Layout.fillWidth: true

                }
            }
        }

        drawerContentItem: ColumnLayout {

            ThumbnailSource {
                id: thumbnailSource
                videoId: drawer.videoId
            }

            Kirigami.BasicListItem{
                Layout.topMargin: 10
                label: i18n("Play Next")
                icon.name: "go-next"
                onClicked: {
                    UserPlaylistModel.playNext(drawer.videoId, drawer.songTitle, drawer.artists)
                    drawer.close()
                }
            }
            Kirigami.BasicListItem{

                label: i18n("Add to queue")
                icon.name: "media-playlist-append"
                onClicked: {
                    UserPlaylistModel.append(drawer.videoId, drawer.songTitle, drawer.artists)
                    drawer.close()
                }
            }
            Kirigami.BasicListItem{
                readonly property QtObject favouriteWatcher: Library.favouriteWatcher(drawer.videoId)

                label: favouriteWatcher ? (favouriteWatcher.isFavourite ? i18n("Remove Favourite"): i18n("Add Favourite")): ""
                icon.name: favouriteWatcher ? (favouriteWatcher.isFavourite ? "starred-symbolic" : "non-starred-symbolic") : ""
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

            Kirigami.BasicListItem{
                readonly property QtObject wasPlayedWatcher: Library.wasPlayedWatcher(drawer.videoId)
                
                label: i18n("Remove from History")
                icon.name: "list-remove"
                onClicked: {
                    Library.removePlaybackHistoryItem(drawer.videoId)
                    drawer.close()
                }
                visible: wasPlayedWatcher ? wasPlayedWatcher.wasPlayed : false
                enabled: wasPlayedWatcher ? wasPlayedWatcher.wasPlayed : false
            }

            Kirigami.BasicListItem{
                readonly property QtObject wasPlayedWatcher: Library.wasPlayedWatcher(drawer.videoId)

                label: i18n("Add to playlist")
                icon: "media-playlist-append"
                onClicked: {
                    playlistDialog.open()
                    drawer.close()
                }
            }

            // Page specific actions //

            Repeater {
                model: root.pageSpecificActions
                delegate: Kirigami.BasicListItem{
                    required property var modelData
                    label: modelData.text
                    icon: modelData.icon.name
                    onClicked: {
                        modelData.triggered()
                        drawer.close()
                    }
                }
            }


            Kirigami.BasicListItem{
                readonly property QtObject wasPlayedWatcher: Library.wasPlayedWatcher(drawer.videoId)

                label: i18n("Share Song")
                icon: "emblem-shared-symbolic"
                onClicked: {
                    shareMenu.open()
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


        Controls.MenuSeparator{}

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
        Controls.MenuItem{
            readonly property QtObject wasPlayedWatcher: Library.wasPlayedWatcher(menu.videoId)
            text: i18n("Remove from History")
            icon.name: "list-remove"
            onTriggered: {
                Library.removePlaybackHistoryItem(menu.videoId)
            }
            enabled: wasPlayedWatcher ? wasPlayedWatcher.wasPlayed : false
            visible: wasPlayedWatcher ? wasPlayedWatcher.wasPlayed : false
        }
        Controls.MenuItem {
            text: i18n("Add to playlist")
            icon.name: "media-playlist-append"
            onTriggered: playlistDialog.open()
        }
        // Page specific actions //

        Controls.MenuSeparator{ visible: instantiator.count !== 0 }

        Instantiator {
            id: instantiator
            model: root.pageSpecificActions
            onObjectAdded: menu.insertItem(index+8, object)
            onObjectRemoved: menu.removeItem(object)
            delegate: Controls.MenuItem {
                required property var modelData
                icon.name: modelData.icon.name
                text:modelData.text
                onTriggered: modelData.triggered()
            }
        }


        Controls.MenuSeparator{}

        Controls.MenuItem {
            text: i18n("Share Song")
            icon.name: "emblem-shared-symbolic"
            onTriggered: shareMenu.open()
        }
    }
}
