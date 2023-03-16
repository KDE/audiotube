// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL


import QtQuick 2.1
import QtQuick.Controls 2.12 as Controls
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.14 as Kirigami
import org.kde.ytmusic 1.0

Kirigami.ScrollablePage {
    id: root
    property alias playlistId: playlistModel.playlistId
    SongMenu {
        id:menu
        pageSpecificActions:[
            Kirigami.Action {
                text: i18n("Remove from Playlist")
                icon.name: "list-remove"
                onTriggered:{
                    playlistModel.removeSong(menu.videoId, root.playlistId)
                }
            }
        ]
    }
    ListView {
        Kirigami.PlaceholderMessage {
            text: i18n("This playlist is still empty")
            anchors.centerIn: parent
            visible: parent.count < 1
        }

        model: LocalPlaylistModel {
            id: playlistModel
        }

        header: RowLayout{
            spacing: Kirigami.Units.MediumSpacing

            Controls.ToolButton {
                text: i18n("Play")
                icon.name: "media-playback-start"
                onClicked:  UserPlaylistModel.playLocalPlaylist(playlistModel, false)

            }

            Controls.ToolButton {
                text: i18n("Shuffle")
                icon.name: "shuffle"
                onClicked: UserPlaylistModel.playLocalPlaylist(playlistModel, true)

            }
            Controls.ToolButton {
                text: i18n("Append to queue")
                icon.name: "media-playlist-append"
                onClicked:  UserPlaylistModel.appendLocalPlaylist(playlistModel, false)

            }
            Item {
                Layout.fillWidth: true
            }
        }



        delegate: Kirigami.SwipeListItem {
            id: delegateItem
            alwaysVisibleActions:true

            required property string title
            required property string videoId
            required property var artists
            required property string artistsDisplayString
            required property int index

            onClicked: {
                play(videoId)
            }

            RowLayout {
                ThumbnailSource {
                    id: thumbnailSource

                    videoId: delegateItem.videoId
                }

                RoundedImage {
                    source: thumbnailSource.cachedPath
                    height: 35
                    width: height
                    radius: 5
                }
                ColumnLayout {
                    Controls.Label {
                        text: delegateItem.title
                        Layout.fillWidth: true
                        elide: Qt.ElideRight

                    }
                    Controls.Label {
                        Layout.fillWidth: true
                        color: Kirigami.Theme.disabledTextColor
                        text: delegateItem.artistsDisplayString
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

    }
}
