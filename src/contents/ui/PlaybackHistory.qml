// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls

import org.kde.kirigami 2.15 as Kirigami

import org.kde.ytmusic 1.0

Kirigami.ScrollablePage {
    id: playbackHistoryPage
    title: i18n("Unknown list of songs")

    property QtObject dataModel

    ListView {
        id: listView
        reuseItems: true

        Kirigami.PlaceholderMessage {
            visible: listView.count < 1
            text: i18n("No songs here yet")
            anchors.centerIn: parent
        }

        model: playbackHistoryPage.dataModel

        delegate: Kirigami.SwipeListItem {
            id: delegateItem
            required property string title
            required property string videoId
            required property string artists

            RowLayout {
                ThumbnailSource {
                    id: thumbnailSource

                    videoId: delegateItem.videoId
                }

                Item {
                    width: Kirigami.Units.gridUnit * 3
                    height: Kirigami.Units.gridUnit * 3

                    Image {
                        sourceSize: "200x200"
                        source: thumbnailSource.cachedPath
                        fillMode: Image.PreserveAspectCrop

                        width: Kirigami.Units.gridUnit * 3
                        height: Kirigami.Units.gridUnit * 3

                        Layout.alignment: Qt.AlignLeft
                    }
                }

                Controls.Label {
                    text: delegateItem.title
                    Layout.fillWidth: true
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
        }
    }
}
