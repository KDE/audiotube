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
    property string channelId
    property string radioId
    property string shuffleId
    title: artistModel.title

    SongMenu {
        id: menu
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
            onTriggered: playPlaylist(shuffleId)
            text: "Shuffle"
        }
        leftAction: Kirigami.Action {
            icon.name: "radio"
            onTriggered: playPlaylist(radioId)
            text: "Radio"
        }

    }

    ListView {
        id: songList
        footer: Item { height: 60 }
        header: ListHeader {
            visibleActions: [
                Kirigami.Action {
                    text: i18n("Radio")
                    icon.name: "radio"
                    onTriggered: playPlaylist(radioId)
                    visible: radioId
                },
                Kirigami.Action {
                    text: i18n("Shuffle")
                    icon.name: "media-playlist-shuffle"
                    onTriggered: playPlaylist(shuffleId)
                    visible: shuffleId
                }
            ]
            overflowActions: [
                Kirigami.Action {
                    text: i18n("Open in Browser")
                    icon.name: "internet-services"
                    onTriggered: Qt.openUrlExternally(artistModel.webUrl)
                },
                Kirigami.Action {
                    text: i18n("Share")
                    icon.name: "emblem-shared-symbolic"
                    onTriggered: {
                        openShareMenu(artistModel.title, artistModel.webUrl)
                    }
                }
            ]
            title: artistModel.title
            imageSourceURL: artistModel.thumbnailUrl
            subtitle: i18n("Artist")
            rounded: true
            width: songList.width

        }

        reuseItems: true

        model: ArtistModel {
            id: artistModel

            channelId: root.channelId

            onOpenAlbum: (browseId) => {
                pageStack.push("qrc:/qt/qml/org/kde/audiotube/contents/ui/AlbumPage.qml", {
                    "browseId": browseId
                })
            }

            onOpenSong: (videoId) => {
                play(videoId)
            }

            onOpenVideo: (videoId, title) => {
                videoPlayMessage.text = i18n("Video playback is not supported yet. Do you want to play only the audio of \"%1\"?", title)
                videoPlayMessage.visible = true
                videoPlayMessage.okAction = () => {
                    play(videoId)
                    videoPlayMessage.visible = false
                }
                videoPlayMessage.cancelAction = () => {
                    videoPlayMessage.visible = false
                }
            }
        }

        section.property: "type"
        section.delegate: Kirigami.ListSectionHeader {
            width: parent.width

            text: {
                switch(parseInt(section)) {
                case ArtistModel.Album:
                    return i18n("Albums")
                case ArtistModel.Single:
                    return i18n("Singles")
                case ArtistModel.Song:
                    return i18n("Songs")
                case ArtistModel.Video:
                    return i18n("Videos")
                }
            }
        }

        delegate: Controls.ItemDelegate {
            id: delegateItem

            required property string title
            required property int type
            required property int index
            required property var artists
            required property string videoId
            required property string thumbnailUrl

            width: parent.width

            contentItem: MouseArea {
                implicitHeight: content.implicitHeight
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: (mouse) => {
                    if (mouse.button === Qt.RightButton) {
                       if (type === ArtistModel.Song) {
                           menu.openForSong(delegateItem.videoId, delegateItem.title, delegateItem.artists, artistModel.title)
                        }
                   } else if (mouse.button === Qt.LeftButton) {
                        artistModel.triggerItem(index)
                   }
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

                    Controls.Label {
                        Layout.fillWidth: true
                        text: delegateItem.title
                        elide: Qt.ElideRight
                    }

                    Controls.ToolButton {
                        icon.name: "overflow-menu"
                        text: i18n("More")
                        display: Controls.AbstractButton.IconOnly
                        visible: type === ArtistModel.Song
                        onClicked: menu.openForSong(delegateItem.videoId, delegateItem.title, delegateItem.artists, artistModel.title)
                    }
                }
            }
        }

        Controls.BusyIndicator {
            anchors.centerIn: parent
            visible: artistModel.loading
        }
    }

    footer: ConfirmationMessage {
        id: videoPlayMessage
    }
}
