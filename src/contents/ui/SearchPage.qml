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

            onOpenArtist: (channelId) => {
                pageStack.push("qrc:/ArtistPage.qml", {
                    "channelId": channelId
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
                askPlayVideoAudio(videoId, title)
            }
        }
        delegate: Kirigami.SwipeListItem {
            id: delegateItem
            required property int index
            required property string title
            required property int type
            required property string videoId
            required property var artists
            required property string radioPlaylistId

            RowLayout {
                Layout.fillHeight: true
                Kirigami.Icon {
                    Layout.fillHeight: true
                    source: {
                        switch (type) {
                        case SearchModel.Artist:
                            return "view-media-artist"
                        case SearchModel.Album:
                            return "media-album-cover"
                        case SearchModel.Playlist:
                            return "view-media-playlist"
                        case SearchModel.Song:
                            return "emblem-music-symbolic"
                        case SearchModel.Video:
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
                    text: i18n("Play next")
                    visible: delegateItem.type === SearchModel.Song || delegateItem.type == SearchModel.Video
                    onTriggered: UserPlaylistModel.playNext(delegateItem.videoId, delegateItem.title, delegateItem.artists)
                },
                Kirigami.Action {
                    icon.name: "media-playlist-append"
                    text: i18n("Add to Playlist")
                    visible: delegateItem.type === SearchModel.Song || delegateItem.type == SearchModel.Video
                    onTriggered: UserPlaylistModel.append(delegateItem.videoId, delegateItem.title, delegateItem.artists)
                },
                Kirigami.Action {
                    icon.name: "radio"
                    text: i18n("Radio")
                    visible: delegateItem.type === SearchModel.Artist && delegateItem.radioPlaylistId
                    onTriggered: playPlaylist(delegateItem.radioPlaylistId)
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
}
