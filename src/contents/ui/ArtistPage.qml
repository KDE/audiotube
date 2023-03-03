// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.14 as Controls
import QtQuick.Layouts 1.3

import org.kde.ytmusic 1.0

Kirigami.ScrollablePage {
    property alias channelId: artistModel.channelId
    property string radioId
    property string shuffleId
    title: artistModel.title
    ShareMenu {
        id: shareMenu
        inputTitle: "Title"
        url: artistModel.webUrl
    }
    SongMenu {
        id: menu
    }
    ListView {
        id: songList

        header: ListHeader {
            visibleActions: [
                Kirigami.Action {
                    text: i18n("Radio")
                    icon.name: "radio"
                    onTriggered: playPlaylist(radioId)
                },
                Kirigami.Action {
                    text: i18n("Shuffle")
                    icon.name: "media-playlist-shuffle"
                    onTriggered: playPlaylist(shuffleId)
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
                        shareMenu.open()
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

            onOpenAlbum: (browseId) => {
                pageStack.push("qrc:/AlbumPage.qml", {
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

        delegate: Kirigami.SwipeListItem {
            id: delegateItem
            alwaysVisibleActions:true

            required property string title
            required property int type
            required property int index
            required property var artists
            required property string videoId
            required property string thumbnailUrl

            RowLayout {
                Layout.fillHeight: true
                RoundedImage {
                    source: delegateItem.thumbnailUrl
                    height: 35
                    width: height
                    radius:5
                }

                Controls.Label {
                    Layout.fillWidth: true
                    text: title
                    elide: Qt.ElideRight
                }
            }

            actions: [
                Kirigami.Action {
                    icon.name: "view-more-horizontal-symbolic"
                    text: i18n("More")
                    visible: type === ArtistModel.Song
                    onTriggered: menu.openForSong(delegateItem.videoId, delegateItem.title, delegateItem.artists, artistModel.title)
                }
            ]

            onClicked: artistModel.triggerItem(index)
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
