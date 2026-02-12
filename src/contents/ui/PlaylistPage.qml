// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

import org.kde.audiotube

Kirigami.ScrollablePage {
    id: root

    required property string playlistId

    title: playlistModel.title

    SongMenu {
        id: menu
        parentItem: root.Controls.ApplicationWindow.window
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
            onTriggered: {
                applicationWindow().playShufflePlaylist(playlistModel.playlistId)
            }
        }
        leftAction: Kirigami.Action {
            icon.name: "media-playback-start"
            text: i18n("Play")
            onTriggered: {
                applicationWindow().playPlaylist(playlistModel.playlistId)
            }
        }
    }

    PlaylistImporter {
        id: localPlaylistsModel
        onImportFinished: {
            applicationWindow().showPassiveNotification(i18n("Playlist successfully imported"))
        }
    }

    ListView {
        id: songList
        footer: Item { height: 60 }
        header: ListHeader {
            visibleActions: [
                Kirigami.Action {
                    icon.name: "media-playback-start"
                    text: i18n("Play")
                    onTriggered: {
                        applicationWindow().playPlaylist(playlistModel.playlistId)
                    }
                },
                Kirigami.Action {
                    icon.name: "media-playlist-shuffle"
                    text: i18n("Shuffle")
                    onTriggered: {
                        applicationWindow().playShufflePlaylist(playlistModel.playlistId)
                    }
                }
            ]
            overflowActions: [
                Kirigami.Action {
                    text: i18n("Append to queue")
                    icon.name: "media-playlist-append"
                    onTriggered: UserPlaylistModel.appendPlaylist(playlistModel)
                },
                Kirigami.Action {
                    text: i18n("Import Playlist")
                    icon.name: "document-save"
                    onTriggered: localPlaylistsModel.importPlaylist(playlistModel.webUrl)
                },
                Kirigami.Action {
                    text: i18n("Open in Browser")
                    icon.name: "internet-services"
                    onTriggered: Qt.openUrlExternally(playlistModel.webUrl)
                },
                Kirigami.Action {
                    text: i18n("Share")
                    icon.name: "emblem-shared-symbolic"
                    onTriggered:{
                        openShareMenu(playlistModel.title, playlistModel.webUrl)
                    }
                }

            ]
            title: playlistModel.title
            imageSourceURL: playlistModel.thumbnailUrl
            width: songList.width
            subtitle: i18n("Playlist")

        }

        model: PlaylistModel {
            id: playlistModel

            playlistId: root.playlistId
        }

        reuseItems: true

        delegate: Controls.ItemDelegate {
            id: delegateItem

            required property string title
            required property string videoId
            required property var artists
            required property string thumbnailUrl
            required property string artistsDisplayString
            required property int index

            width: parent.width

            contentItem: MouseArea {
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

                    RoundedImage {
                        source: delegateItem.thumbnailUrl
                        height: 35
                        width: height
                        radius: 5
                    }

                    ColumnLayout {
                        Controls.Label {
                            Layout.fillWidth: true
                            text: title
                            elide: Qt.ElideRight
                        }

                        Controls.Label {
                            Layout.fillWidth: true
                            visible: delegateItem.artistsDisplayString
                            color: Kirigami.Theme.disabledTextColor
                            text: delegateItem.artistsDisplayString
                            elide: Qt.ElideRight
                        }
                    }

                    Controls.ToolButton {
                        icon.name: "overflow-menu"
                        text: i18n("More")
                        display: Controls.AbstractButton.IconOnly
                        onClicked: menu.openForSong(delegateItem.videoId, delegateItem.title, delegateItem.artists, delegateItem.artistsDisplayString)
                    }
                }
            }
        }

        Controls.BusyIndicator {
            anchors.centerIn: parent
            visible: playlistModel.loading
        }
    }
}
