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
    SongMenu {
        id:menu
    }
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
                    }
                    Controls.Label {
                        Layout.fillWidth: true
                        color: Kirigami.Theme.disabledTextColor
                        text: delegateItem.artists
                    }
                }
            }

            actions: [
                Kirigami.Action {
                    icon.name: "view-more-horizontal-symbolic"
                    text: i18n("More")
                    onTriggered: menu.openForSong(delegateItem.videoId, delegateItem.title, delegateItem.artists, delegateItem.artists)
                }
            ]
        }
    }
}
