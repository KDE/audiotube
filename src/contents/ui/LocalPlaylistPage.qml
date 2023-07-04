// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL


import QtQuick 2.15
import QtQuick.Controls 2.12 as Controls
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.14 as Kirigami
import org.kde.ytmusic 1.0

import "components"

Kirigami.ScrollablePage {
    id: root
    readonly property bool isWidescreen: width >= Kirigami.Units.gridUnit * 30
    property alias playlistId: playlistModel.playlistId
    SongMenu {
        id:menu
        pageSpecificActions:[
            Kirigami.Action {
                text: i18n("Remove from Playlist")
                icon.name: "list-remove"
                onTriggered:{
                    playlistModel.removeSong(menu.videoId, root.playlistId)
                }
            }
        ]
    }

    DoubleActionButton {
        id: action
        visible: false
        property bool shown
        shown: !root.flickable.atYBeginning
        onShownChanged:
            if(shown){
                visible = true
                appear.running = true
            } else {
                disappear.running = true
            }

        parent: overlay
        x: root.width - width - margin
        y: root.height - height - margin
        NumberAnimation on y {
            id: appear
            easing.type: Easing.InCubic
            running: false
            from: root.height
            to: root.height - action.height - action.margin
            duration: 100
        }
        NumberAnimation on y {
            id: disappear
            easing.type: Easing.OutCubic
            running: false
            from: root.height - action.height - action.margin
            to: root.height
            duration: 100
            onFinished: action.visible = false
        }
        rightAction: Kirigami.Action {
            icon.name: "media-playlist-shuffle"
            text: i18n("Shuffle")
            onTriggered: UserPlaylistModel.playLocalPlaylist(playlistModel, true)
        }
        leftAction: Kirigami.Action {
            icon.name: "media-playback-start"
            text: i18n("Play")
            onTriggered: UserPlaylistModel.playLocalPlaylist(playlistModel, false)
        }

    }

    ListView {
        footer: Item { height: 60 }
        Kirigami.PlaceholderMessage {
            text: i18n("This playlist is still empty")
            anchors.centerIn: parent
            visible: parent.count < 1
        }

        model: LocalPlaylistModel {
            id: playlistModel
        }

        header: RowLayout{
            anchors.right: parent.right
            anchors.left: parent.left
            spacing: Kirigami.Units.MediumSpacing

            Controls.ToolButton {
                text: i18n("Play")
                icon.name: "media-playback-start"
                onClicked:  UserPlaylistModel.playLocalPlaylist(playlistModel, false)

            }

            Controls.ToolButton {
                text: i18n("Shuffle")
                icon.name: "shuffle"
                onClicked: UserPlaylistModel.playLocalPlaylist(playlistModel, true)

            }
            Controls.ToolButton {
                text: i18n("Append to queue")
                icon.name: "media-playlist-append"
                visible: isWidescreen
                onClicked:  UserPlaylistModel.appendLocalPlaylist(playlistModel, false)
            }
            Controls.ToolButton {
                text: i18n("Export")
                icon.name: "document-save"
                visible: isWidescreen
                onClicked: {
                    applicationWindow().localPlaylistModel.playlistId = playlistId
                    applicationWindow().fileDialog.exportPlaylist()
                }
            }
            Item {
                Layout.fillWidth: true
                visible: !isWidescreen
            }
            Controls.ToolButton {
                Layout.fillHeight: true
                Layout.rightMargin: 0.73*Kirigami.Units.gridUnit
                icon.name: "view-more-symbolic"
                visible:!isWidescreen
                onPressed: Kirigami.Settings.isMobile? drawer.open() : menu.popup()
                Controls.Menu {
                    id: menu
                    Controls.MenuItem {
                        text: i18n("Append to queue")
                        icon.name: "media-playlist-append"
                        onTriggered: UserPlaylistModel.appendLocalPlaylist(playlistModel, false)
                    }
                    Controls.MenuItem {
                        text: i18n("Export")
                        icon.name: "document-save"
                        onTriggered: {
                            applicationWindow().localPlaylistModel.playlistId = playlistId
                            applicationWindow().fileDialog.exportPlaylist()
                        }
                    }
                }

                BottomDrawer{
                    id: drawer
                    drawerContentItem: ColumnLayout {
                        Kirigami.BasicListItem {
                            label: i18n("Append to queue")
                            icon: "media-playlist-append"
                            onClicked: {
                                UserPlaylistModel.appendLocalPlaylist(playlistModel, false)
                                favDrawer.close()
                            }
                        }
                        Kirigami.BasicListItem {
                            label: i18n("Export")
                            icon: "document-save"
                            onClicked: {
                                applicationWindow().localPlaylistModel.playlistId = playlistId
                                applicationWindow().fileDialog.exportPlaylist()
                                favDrawer.close()
                            }
                        }
                        Item{
                            Layout.fillHeight: true
                        }
                    }
                }

            }

            Item {
                Layout.fillWidth: true
                visible: isWidescreen
            }
        }



        delegate: Kirigami.SwipeListItem {
            id: delegateItem
            alwaysVisibleActions:true

            required property string title
            required property string videoId
            required property var artists
            required property string artistsDisplayString
            required property int index

            MouseArea {
                implicitHeight: content.implicitHeight
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: if (mouse.button === Qt.RightButton) {
                               menu.openForSong(delegateItem.videoId, delegateItem.title, delegateItem.artists, delegateItem.artistsDisplayString)
                          } else if (mouse.button === Qt.LeftButton) {
                               play(videoId)
                          }
                RowLayout {
                    id: content
                    anchors.fill: parent
                    ThumbnailSource {
                        id: thumbnailSource

                        videoId: delegateItem.videoId
                    }

                    RoundedImage {
                        source: thumbnailSource.cachedPath
                        height: 35
                        width: height
                        radius: 5
                    }
                    ColumnLayout {
                        Controls.Label {
                            text: delegateItem.title
                            Layout.fillWidth: true
                            elide: Qt.ElideRight

                        }
                        Controls.Label {
                            Layout.fillWidth: true
                            color: Kirigami.Theme.disabledTextColor
                            text: delegateItem.artistsDisplayString
                            elide: Qt.ElideRight

                        }
                    }
                }
            }
            actions: [
                Kirigami.Action {
                    icon.name: "overflow-menu"
                    text: i18n("More")
                    onTriggered: menu.openForSong(delegateItem.videoId, delegateItem.title, delegateItem.artists, delegateItem.artistsDisplayString)
                }
            ]
        }

    }
}
