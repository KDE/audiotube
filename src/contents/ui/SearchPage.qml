// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14 as Controls
import org.kde.kirigami 2.14 as Kirigami

import org.kde.ytmusic 1.0

Kirigami.ScrollablePage {
    property alias searchQuery: searchModel.searchQuery

    title: searchQuery

    ListView {
        reuseItems: true

        section.delegate: Kirigami.ListSectionHeader {
            required property string section
            text: {
                switch (parseInt(section)) {
                case SearchModel.Album:
                    return i18n("Albums")
                case SearchModel.Artist:
                    return i18n("Artists")
                case SearchModel.Playlist:
                    return i18n("Playlists")
                case SearchModel.Song:
                    return i18n("Songs")
                case SearchModel.Video:
                    return i18n("Videos")
                }

                return i18n("Unknown")
            }
        }
        section.property: "type"

        model: SearchModel {
            id: searchModel

            onOpenAlbum: (browseId) => {
                pageStack.push("qrc:/AlbumPage.qml", {
                    "browseId": browseId
                })
            }

            onOpenArtist: (channelId, radioId, shuffleId) => {
                pageStack.push("qrc:/ArtistPage.qml", {
                    "channelId": channelId,
                    "radioId": radioId,
                    "shuffleId": shuffleId
                })
            }

            onOpenPlaylist: (browseId) => {
                pageStack.push("qrc:/PlaylistPage.qml", {
                    "playlistId": browseId
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

        SongMenu {
            id: menu
        }

        delegate: Kirigami.SwipeListItem {

            id: delegateItem

            required property int index
            required property string title
            required property int type
            required property string videoId
            required property var artists
            required property string artistsDisplayString
            required property string radioPlaylistId
            required property string thumbnailUrl

            RowLayout {
                Layout.fillHeight: true
                RoundedImage {
                    source: delegateItem.thumbnailUrl
                    height: 35
                    width: height
                    radius: delegateItem.type === SearchModel.Artist?height/2:5
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
                    icon.name: "view-more-horizontal-symbolic"
                    text: i18n("More")
                    visible: delegateItem.type === SearchModel.Song || delegateItem.type === SearchModel.Video
                    onTriggered: menu.openForSong(delegateItem.videoId, delegateItem.title, delegateItem.artists, delegateItem.artistsDisplayString)
                }
            ]

            onClicked: searchModel.triggerItem(index)
        }
        Controls.BusyIndicator {
            anchors.centerIn: parent
            visible: searchModel.loading
        }
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            width: parent.width - (Kirigami.Units.largeSpacing * 4)

            visible: parent.count === 0 && !searchModel.loading

            text: i18n("Find music on YouTube Music")
            helpfulAction: Kirigami.Action {
                onTriggered: searchField.forceActiveFocus()
                text: i18n("Search")
                icon.name: "search"
            }
        }
    }

    footer: ConfirmationMessage {
        id: videoPlayMessage
    }
}
