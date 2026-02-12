// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami as Kirigami
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.audiotube

Kirigami.ScrollablePage {
    id: root
    property string browseId
    title: albumModel.title

    DoubleActionButton {
        id: action
        visible: false
        property bool shown
        shown: !root.flickable.atYBeginning && albumModel.playlistId
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
            onTriggered: applicationWindow().playShufflePlaylist(albumModel.playlistId)
            text: "Shuffle"
        }
        leftAction: Kirigami.Action {
            icon.name: "media-playback-start"
            onTriggered: applicationWindow().playPlaylist(albumModel.playlistId)
            text: "Play"
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
                    visible: albumModel.playlistId
                    onTriggered: {
                        applicationWindow().playPlaylist(albumModel.playlistId)
                    }
                },
                Kirigami.Action {
                    icon.name: "media-playlist-shuffle"
                    text: i18n("Shuffle")
                    visible: albumModel.playlistId
                    onTriggered: {
                        applicationWindow().playShufflePlaylist(albumModel.playlistId)
                    }
                }
            ]
            overflowActions: [
                Kirigami.Action {
                    text: i18n("Append to queue")
                    icon.name: "media-playlist-append"
                    onTriggered: UserPlaylistModel.appendAlbum(albumModel)
                    visible: albumModel.playlistId
                },
                Kirigami.Action {
                    text: i18n("Open in Browser")
                    icon.name: "internet-services"
                    onTriggered: Qt.openUrlExternally(albumModel.webUrl)
                    visible: albumModel.playlistId
                },
                Kirigami.Action {
                    text: i18n("Share")
                    icon.name: "emblem-shared-symbolic"
                    onTriggered:{
                        openShareMenu(albumModel.title, albumModel.webUrl)
                        visible: albumModel.playlistId
                    }
                }

            ]
            title: albumModel.title
            imageSourceURL: albumModel.thumbnailUrl
            subtitle: i18nc("%1 is the artist(s) featured in the album", "Album • %1" , albumModel.artists)
            width: songList.width
        }


        reuseItems: true

        model: AlbumModel {
            id: albumModel

            browseId: root.browseId
        }

        SongMenu {
            id: menu
            parentItem: root.Controls.ApplicationWindow.window
        }

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
                                play(delegateItem.videoId)
                           }
                RowLayout {
                    id: content
                    anchors.fill: parent
                    Controls.Label {
                        text: (delegateItem.index + 1)
                        font.bold: true
                        leftPadding: 5
                        color: Kirigami.Theme.disabledTextColor
                        Layout.preferredWidth: 30
                    }

                    Controls.Label {
                        Layout.fillWidth: true
                        text: delegateItem.title
                        elide: Qt.ElideRight
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
            visible: albumModel.loading
        }
    }
}
