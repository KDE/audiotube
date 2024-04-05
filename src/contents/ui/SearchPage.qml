// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14 as Controls
import org.kde.kirigami 2.14 as Kirigami

import org.kde.ytmusic 1.0

Kirigami.ScrollablePage {
    id: searchPage
    property alias searchQuery: searchModel.searchQuery
    objectName: "searchPage"
    title: listView.count === 0 && !searchModel.loading? i18n("Previous Searches:") : searchQuery

    ListView {
        reuseItems: true

        section.delegate: Kirigami.ListSectionHeader {
            width: parent.width

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
                case SearchModel.TopResult:
                    return i18n("Top Results")
                }

                return i18n("Unknown")
            }
        }
        section.property: "type"
        id: listView
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

        delegate: Controls.ItemDelegate {
            id: delegateItem

            required property int index
            required property string title
            required property int type
            required property string videoId
            required property var artists
            required property string artistsDisplayString
            required property string radioPlaylistId
            required property string thumbnailUrl

            width: parent.width

            contentItem: MouseArea {
                implicitHeight: content.implicitHeight
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: (mouse) => {
                    if (mouse.button === Qt.RightButton) {
                        if (delegateItem.videoId && delegateItem.title && delegateItem.artists && delegateItem.artistsDisplayString) {
                            menu.openForSong(delegateItem.videoId, delegateItem.title, delegateItem.artists, delegateItem.artistsDisplayString)
                        }
                    } else if (mouse.button === Qt.LeftButton) {
                        searchModel.triggerItem(index)
                    }
                }
                RowLayout {
                    id: content
                    anchors.fill: parent
                    RoundedImage {
                        source: delegateItem.thumbnailUrl
                        height: 35
                        width: height
                        radius: delegateItem.type === SearchModel.Artist ? height / 2 : 5
                    }

                    ColumnLayout {
                        Layout.fillWidth: true

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
                        display: Controls.AbstractButton.IconOnly
                        text: i18n("More")
                        visible: delegateItem.videoId && delegateItem.title && delegateItem.artists && delegateItem.artistsDisplayString
                        onClicked: menu.openForSong(delegateItem.videoId, delegateItem.title, delegateItem.artists, delegateItem.artistsDisplayString)
                    }
                }
            }
        }

        Controls.BusyIndicator {
            anchors.centerIn: parent
            visible: searchModel.loading
        }
    }

    footer: ConfirmationMessage {
        id: videoPlayMessage
    }
}
