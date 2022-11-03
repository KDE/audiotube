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

    actions {
        main: Kirigami.Action {
            icon.name: "media-playback-start"
            text: i18n("Play")
            onTriggered: {
                applicationWindow().playPlaylist(albumModel.playlistId)
            }
        }
        right: Kirigami.Action {
            icon.name: "media-playlist-shuffle"
            text: i18n("Shuffle")
            onTriggered: {
                applicationWindow().playShufflePlaylist(albumModel.playlistId)
            }
        }
    }

    contextualActions: [
        Kirigami.Action {
            text: i18n("Open in Browser")
            icon.name: "internet-services"
            onTriggered: Qt.openUrlExternally(albumModel.webUrl)
        }

    ]

    ListView {
        header: Kirigami.ItemViewHeader {
            backgroundImage.source: albumModel.thumbnailUrl
            title: albumModel.title
        }

        reuseItems: true

        model: AlbumModel {
            id: albumModel
        }
        delegate: Kirigami.SwipeListItem {
            id: delegateItem

            required property string title
            required property string videoId
            required property var artists
            required property string thumbnailUrl

            RowLayout {
                Layout.fillHeight: true
                Kirigami.Icon {
                    Layout.fillHeight: true
                    source: delegateItem.thumbnailUrl
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
                    onTriggered: UserPlaylistModel.playNext(delegateItem.videoId, delegateItem.title, delegateItem.artists)
                },
                Kirigami.Action {
                    icon.name: "media-playlist-append"
                    text: i18n("Add to Playlist")
                    onTriggered: UserPlaylistModel.append(delegateItem.videoId, delegateItem.title, delegateItem.artists)
                }
            ]

            onClicked: play(videoId)
        }

        Controls.BusyIndicator {
            anchors.centerIn: parent
            visible: albumModel.loading
        }
    }
}
