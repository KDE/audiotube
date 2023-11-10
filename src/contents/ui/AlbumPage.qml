// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.14 as Controls
import QtQuick.Layouts 1.3

import org.kde.ytmusic 1.0

import "components"

Kirigami.ScrollablePage {
    id: root
    property string browseId
    title: albumModel.title

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
                    onTriggered: {
                        applicationWindow().playPlaylist(albumModel.playlistId)
                    }
                },
                Kirigami.Action {
                    icon.name: "media-playlist-shuffle"
                    text: i18n("Shuffle")
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
                },
                Kirigami.Action {
                    text: i18n("Open in Browser")
                    icon.name: "internet-services"
                    onTriggered: Qt.openUrlExternally(albumModel.webUrl)
                },
                Kirigami.Action {
                    text: i18n("Share")
                    icon.name: "emblem-shared-symbolic"
                    onTriggered:{
                        openShareMenu(albumModel.title, albumModel.webUrl)
                    }
                }

            ]
            title: albumModel.title
            imageSourceURL: albumModel.thumbnailUrl
            subtitle: i18n("Album • %1" , albumModel.artists)
            width: songList.width
        }


        reuseItems: true

        model: AlbumModel {
            id: albumModel

            browseId: root.browseId
        }
        SongMenu {
            id: menu
        }
        delegate: Kirigami.SwipeListItem {
            id: delegateItem
            alwaysVisibleActions:true

            required property string title
            required property string videoId
            required property var artists
            required property string thumbnailUrl
            required property string artistsDisplayString
            required property int index
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
                        text: (index + 1)
                        font.bold: true
                        leftPadding: 5
                        color: Kirigami.Theme.disabledTextColor
                        Layout.preferredWidth: 30
                    }

                    Controls.Label {
                        Layout.fillWidth: true
                        text: title
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
        }

        Controls.BusyIndicator {
            anchors.centerIn: parent
            visible: albumModel.loading
        }
    }
}
