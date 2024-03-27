// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL


import QtQuick 2.15
import QtQuick.Controls 2.12 as Controls
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.14 as Kirigami
import org.kde.kirigami.delegates as KirigamiDelegates
import org.kde.ytmusic 1.0

import "components"

Kirigami.ScrollablePage {
    id: root
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
        onShownChanged: {
            if (shown) {
                visible = true
                appear.running = true
            } else {
                disappear.running = true
            }
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
                onClicked:  UserPlaylistModel.appendLocalPlaylist(playlistModel, false)
            }
            Item {
                Layout.fillWidth: true
            }
        }



        delegate: Controls.ItemDelegate {
            id: delegateItem

            required property string title
            required property string videoId
            required property var artists
            required property string artistsDisplayString
            required property int index

            width: parent.width

            contentItem: MouseArea {
                implicitHeight: content.implicitHeight
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: {
                    if (mouse.button === Qt.RightButton) {
                       menu.openForSong(delegateItem.videoId, delegateItem.title, delegateItem.artists, delegateItem.artistsDisplayString)
                    } else if (mouse.button === Qt.LeftButton) {
                       play(videoId)
                    }
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

                    Controls.ToolButton {
                        icon.name: "overflow-menu"
                        text: i18n("More")
                        display: Controls.AbstractButton.IconOnly
                        Layout.alignment: Qt.AlignRight
                        Layout.rightMargin: 5
                        onClicked: menu.openForSong(delegateItem.videoId, delegateItem.title, delegateItem.artists, delegateItem.artistsDisplayString)
                    }
                }
            }
        }
    }
}
