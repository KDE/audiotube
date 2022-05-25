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
            Kirigami.Heading {
                Layout.topMargin: 20
                text: i18n("Playlists")
                Layout.alignment: Qt.AlignLeft
                leftPadding: 15
            }

            // Spacer
            Item {
                visible: !isWidescreen
                Layout.fillWidth: true
            }
            Controls.ToolButton {
                Layout.topMargin: 20
                text: i18n("New Playlist")
                Layout.alignment: Qt.AlignRight
                icon.name: "list-add"
                AddPlaylistDialog{
                    id: addPlaylistDialog
                    model: localPlaylistsModel
                }
                onClicked: addPlaylistDialog.open()
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
            onThumbnailIdsChanged: console.log(thumbnailIds)
            width: grid.cellWidth
            Kirigami.ShadowedRectangle {
                Layout.alignment: Qt.AlignHCenter
                color: Kirigami.Theme.backgroundColor
                id: favCover
                MouseArea {
                    id: favArea
                    anchors.fill: parent
                    onClicked: pageStack.push("qrc:/LocalPlaylistPage.qml", {
                                                         "playlistId": playlistDelegate.playlistId,
                                                         "title": playlistDelegate.title
                                                     })
                    hoverEnabled: !Kirigami.Settings.hasTransientTouchInput
                    onEntered: {
                        if (!Kirigami.Settings.hasTransientTouchInput){
                            favSelected.visible = true
                            playlistTitle.color = Kirigami.Theme.hoverColor
                            playlistSubtitle.color = Kirigami.Theme.hoverColor
                            playlistTitle.font.bold = true
                        }

                    }

                    onExited:{
                        favSelected.visible = false
                        playlistTitle.color = Kirigami.Theme.textColor
                        playlistSubtitle.color = Kirigami.Theme.disabledTextColor
                        playlistTitle.font.bold = false
                    }
                }
                Layout.margins: 15
                Layout.bottomMargin: 5
                Layout.topMargin: 5
                width: 200
                height: 200
                radius: 10
                shadow.size: 15
                shadow.xOffset: 5
                shadow.yOffset: 5
                shadow.color: Qt.rgba(0, 0, 0, 0.2)


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
                PlaylistCover {
                    source1: thumbnailSource1.cachedPath
                    source2: thumbnailSource2.cachedPath
                    source3: thumbnailSource3.cachedPath
                    source4: thumbnailSource4.cachedPath
                    title: playlistDelegate.title
                    height: 200
                    width: height
                    radius: 10
                }

                Rectangle {
                    id: favSelected

                    Rectangle {
                        anchors.fill: parent
                        color: Kirigami.Theme.hoverColor
                        radius: 10
                        opacity: 0.2
                    }


                    visible: false
                    anchors.fill: parent

                    radius: 9

                    border.color: Kirigami.Theme.hoverColor
                    border.width: 2
                    color: "transparent"
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter

                ColumnLayout {
                    Controls.Label {
                        id: playlistTitle
                        text: playlistDelegate.title
                        Layout.maximumWidth: 200
                        Layout.fillWidth: true
                        leftPadding: 15
                        elide: Text.ElideRight

                    }
                    Controls.Label {
                        id: playlistSubtitle
                        Layout.fillWidth: true
                        Layout.maximumWidth: 200
                        leftPadding: 15
                        color: Kirigami.Theme.disabledTextColor
                        text: playlistDelegate.description
                        elide: Text.ElideRight
                    }
                }
                Controls.ToolButton {
                    Layout.fillHeight: true
                    icon.name: "overflow-menu"
                    onClicked:{
                        menu.modelData = playlistDelegate.model
                        drawer.modelData = playlistDelegate.model
                        Kirigami.Settings.isMobile? drawer.open() : menu.popup()
                    }
                }
            }
        }
    }
}



