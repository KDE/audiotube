// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12 as Controls
import org.kde.kirigami 2.12 as Kirigami

import org.kde.ytmusic 1.0

Kirigami.ScrollablePage {
    property alias playlistId: playlistModel.playlistId
    title: playlistModel.title

    actions {
        main: Kirigami.Action {
            icon.name: "media-playback-start"
            text: i18n("Play")
            onTriggered: {
                applicationWindow().playPlaylist(playlistModel.playlistId)
            }
        }
        right: Kirigami.Action {
            icon.name: "media-playlist-shuffle"
            text: i18n("Shuffle")
            onTriggered: {
                applicationWindow().playShufflePlaylist(playlistModel.playlistId)
            }
        }
    }

    ListView {
        header: Kirigami.ItemViewHeader {
            backgroundImage.source: playlistModel.thumbnailUrl
            title: playlistModel.title
        }
        model: PlaylistModel {
            id: playlistModel
        }

        reuseItems: true

        delegate: Kirigami.SwipeListItem {
            id: delegateItem
            required property string title
            required property string videoId
            required property var artists

            RowLayout {
                Layout.fillHeight: true
                Kirigami.Icon {
                    Layout.fillHeight: true
                    source: "emblem-music-symbolic"
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
            visible: playlistModel.loading
        }
    }
}
