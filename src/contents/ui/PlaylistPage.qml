// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12 as Controls
import org.kde.kirigami 2.12 as Kirigami

import org.kde.ytmusic 1.0

import "components"

Kirigami.ScrollablePage {
    id: root
    property alias playlistId: playlistModel.playlistId
    title: playlistModel.title

    SongMenu {
        id: menu
    }
    ShareMenu {
        id: shareMenu
        inputTitle: playlistModel.title
        url: playlistModel.webUrl
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
                    text: i18n("Open in Browser")
                    icon.name: "internet-services"
                    onTriggered: Qt.openUrlExternally(playlistModel.webUrl)
                },
                Kirigami.Action {
                    text: i18n("Share")
                    icon.name: "emblem-shared-symbolic"
                    onTriggered:{
                        shareMenu.open()
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
        }

        reuseItems: true

        delegate: Kirigami.SwipeListItem {
            id: delegateItem
            alwaysVisibleActions:true

            required property string title
            required property string videoId
            required property var artists
            required property string thumbnailUrl
            required property string artistsDisplayString
            required property int index

            RowLayout {
                Layout.fillHeight: true
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
            }

            actions: [
                Kirigami.Action {
                    icon.name: "overflow-menu"
                    text: i18n("More")
                    onTriggered: menu.openForSong(delegateItem.videoId, delegateItem.title, delegateItem.artists, delegateItem.artistsDisplayString)
                }
            ]

            onClicked: play(videoId)
        }

        Controls.BusyIndicator {
            anchors.centerIn: parent
            visible: playlistModel.loading
        }
    }
}
