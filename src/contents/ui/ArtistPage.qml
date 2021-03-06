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

    title: artistModel.title

    ListView {
        header: Kirigami.ItemViewHeader {
            backgroundImage.source: artistModel.thumbnailUrl
            title: artistModel.title
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
                videoPlayMessage.okAction = function() {
                    play(videoId)
                    videoPlayMessage.visible = false
                }
                videoPlayMessage.cancelAction = function() {
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

            required property string title
            required property int type
            required property int index
            required property var artists
            required property string videoId

            RowLayout {
                Layout.fillHeight: true
                Kirigami.Icon {
                    Layout.fillHeight: true
                    source: {
                        switch (type) {
                        case ArtistModel.Album:
                            return "media-album-cover"
                        case ArtistModel.Single:
                        case ArtistModel.Song:
                            return "emblem-music-symbolic"
                        case ArtistModel.Video:
                            return "emblem-videos-symbolic"
                        }
                    }
                }

                Controls.Label {
                    Layout.fillWidth: true
                    text: title
                    elide: Qt.ElideRight
                }
            }

            actions: [
                Kirigami.Action {
                    icon.name: "go-next"
                    text: i18n("Play Next")
                    visible: type === ArtistModel.Song
                    onTriggered: UserPlaylistModel.playNext(delegateItem.videoId, delegateItem.title, delegateItem.artists)
                },
                Kirigami.Action {
                    icon.name: "media-playlist-append"
                    text: i18n("Add to Playlist")
                    visible: type === ArtistModel.Song
                    onTriggered: UserPlaylistModel.append(delegateItem.videoId, delegateItem.title, delegateItem.artists)
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
