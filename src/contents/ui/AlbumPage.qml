// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.14 as Controls
import QtQuick.Layouts 1.3

import org.kde.ytmusic 1.0

Kirigami.ScrollablePage {
    property alias browseId: albumModel.browseId
    title: albumModel.title
    ShareMenu {
        id: shareMenu
        inputTitle: albumModel.title
        url: albumModel.webUrl
    }
    ListView {
        id: songList
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
                        shareMenu.open()
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

            RowLayout {
                Layout.fillHeight: true
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

            actions: [
                Kirigami.Action {
                    icon.name: "view-more-horizontal-symbolic"
                    text: i18n("More")
                    onTriggered: menu.openForSong(delegateItem.videoId, delegateItem.title, delegateItem.artists, delegateItem.artistsDisplayString)
                }
            ]

            onClicked: play(delegateItem.videoId)
        }

        Controls.BusyIndicator {
            anchors.centerIn: parent
            visible: albumModel.loading
        }
    }
}
