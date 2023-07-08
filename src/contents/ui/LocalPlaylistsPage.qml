// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL


import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls

import org.kde.kirigami 2.15 as Kirigami

import org.kde.ytmusic 1.0

import "dialogs"

Kirigami.ScrollablePage {
    id: root
    title: i18n("Playlists")
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    rightPadding: 0
    leftPadding: 15

    RenamePlaylistDialog{
        id: renamePlaylistDialog
        playlistsModel: localPlaylistsModel
    }
    BottomDrawer{
        id: drawer
        property var modelData
        drawerContentItem: ColumnLayout {
            Kirigami.BasicListItem{
                label: i18n("Rename")
                icon: "edit-entry"
                onClicked: {
                    renamePlaylistDialog.modelData = drawer.modelData
                    renamePlaylistDialog.open()
                    drawer.close()
                }
            }
            Kirigami.BasicListItem{
                label: i18n("Delete")
                icon: "delete"
                onClicked: {
                    localPlaylistsModel.deletePlaylist(drawer.modelData.playlistId)
                    drawer.close()
                }
            }
        }
    }
    Controls.Menu {
        id: menu
        property var modelData
        Controls.MenuItem {
            text: i18n("Rename")
            icon.name: "edit-entry"
            onTriggered:{
                renamePlaylistDialog.modelData = menu.modelData
                renamePlaylistDialog.open()
            }
        }
        Controls.MenuItem {
            text: i18n("Delete")
            icon.name: "delete"
            onTriggered:{
                localPlaylistsModel.deletePlaylist(menu.modelData.playlistId)
            }
        }
    }

    GridView {
        id: grid
        header: RowLayout {
            width: parent.width
            spacing: Kirigami.Units.mediumSpacing
            readonly property bool isWidescreen: width >= Kirigami.Units.gridUnit * 25
            Kirigami.Heading {
                Layout.topMargin: 20
                text: i18n("Playlists")
                Layout.alignment: Qt.AlignLeft
                leftPadding: 15
            }
            AddPlaylistDialog {
                id: addPlaylistDialog
                model: localPlaylistsModel
            }
            ImportPlaylistDialog {
                id: importPlaylistDialog
                model: localPlaylistsModel
            }

            // Spacer
            Item {
                visible: isWidescreen
                Layout.fillWidth: true
            }

            // Widescreen layout
            Controls.ToolButton {
                visible: isWidescreen
                Layout.topMargin: 20
                text: i18n("New Playlist")
                Layout.alignment: Qt.AlignRight
                icon.name: "list-add"
                onClicked: addPlaylistDialog.open()
            }
            Controls.ToolButton {
                visible: isWidescreen
                Layout.topMargin: 20
                text: i18n("Import Playlist")
                Layout.alignment: Qt.AlignRight
                icon.name: "download"
                onClicked: importPlaylistDialog.open()
            }

            // Compact layout
            Controls.ToolButton {
                visible: !isWidescreen
                Layout.fillHeight: true
                icon.name: "list-add"
                Layout.topMargin: 20
                Layout.alignment: Qt.AlignRight
                onPressed: Kirigami.Settings.isMobile? favDrawer.open() : favMenu.popup()

                Controls.Menu {
                    id: favMenu
                    Controls.MenuItem {
                        text: i18n("New Playlist")
                        icon.name: "list-add"
                        onTriggered: addPlaylistDialog.open()
                    }
                    Controls.MenuItem {
                        text: i18n("Import Playlist")
                        icon.name: "download"
                        onTriggered: importPlaylistDialog.open()
                    }
                }

                BottomDrawer { 
                    id: favDrawer
                    drawerContentItem: ColumnLayout {
                        Kirigami.BasicListItem{
                            label: i18n("New Playlist")
                            icon: "list-add"
                            onClicked: addPlaylistDialog.open()
                        }
                        Kirigami.BasicListItem{
                            label: i18n("Import Playlist")
                            icon: "download"
                            onClicked: importPlaylistDialog.open()
                        }
                        Item{
                            Layout.fillHeight: true
                        }
                    }
                }
            }
        }

        Layout.fillWidth: true
        Layout.fillHeight: true

        model: LocalPlaylistsModel{ id: localPlaylistsModel }
        cellWidth: grid.width/(Math.floor(grid.width/230))
        cellHeight: 280

        delegate: ColumnLayout {
            id: playlistDelegate
            required property var model
            required property string playlistId
            required property string title
            required property string description
            required property date createdOn
            required property var thumbnailIds

            width: grid.cellWidth

            AlbumCoverItem {
                title: playlistDelegate.title
                subtitle: playlistDelegate.description
                showIcon: false

                Layout.margins: 5
                Layout.alignment: Qt.AlignHCenter

                onClicked: pageStack.push("qrc:/LocalPlaylistPage.qml", {
                    "playlistId": playlistDelegate.playlistId,
                    "title": playlistDelegate.title
                })

                onOptionsClicked: {
                    menu.modelData = playlistDelegate.model
                    drawer.modelData = playlistDelegate.model
                    if (Kirigami.Settings.isMobile) {
                        drawer.open()
                    } else {
                        menu.popup()
                    }
                }

                LocalPlaylistsModel{id:localPlaylistModel}

                ThumbnailSource {
                    id: thumbnailSource1
                    videoId: thumbnailIds[0]
                }
                ThumbnailSource {
                    id: thumbnailSource2
                    videoId: thumbnailIds[1] ?? thumbnailIds[0]
                }
                ThumbnailSource {
                    id: thumbnailSource3
                    videoId: thumbnailIds[2] ?? thumbnailIds[0]
                }
                ThumbnailSource {
                    id: thumbnailSource4
                    videoId: thumbnailIds[3] ?? thumbnailIds[0]
                }
                contentItem: PlaylistCover {
                    source1: thumbnailSource1.cachedPath
                    source2: thumbnailSource2.cachedPath
                    source3: thumbnailSource3.cachedPath
                    source4: thumbnailSource4.cachedPath
                    title: playlistDelegate.title
                    height: 200
                    width: height
                    radius: 10
                }
            }
        }
    }
}



