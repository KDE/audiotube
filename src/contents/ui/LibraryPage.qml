// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.1
import QtQuick.Controls 2.12 as Controls
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.14 as Kirigami

import org.kde.ytmusic 1.0

Kirigami.ScrollablePage {
    
    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.None
    
    // so that there is still a separator, since the header style is none
    Kirigami.Separator {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
    }
    
    ColumnLayout {
        RowLayout {
            Layout.fillWidth: true
            Kirigami.Heading {
                text: i18n("Favourites")
                Layout.alignment: Qt.AlignLeft
            }

            // Spacer
            Item {
                Layout.fillWidth: true
            }

            Controls.ToolButton {
                text: i18n("Show All")
                Layout.alignment: Qt.AlignRight
                icon.name: "arrow-right"
                onClicked: pageStack.layers.push("qrc:/PlaybackHistory.qml", {
                                                     "dataModel": Library.favourites,
                                                     "title": i18n("Favourite Songs")
                                                 })
            }
        }
        Controls.ScrollView {
            Layout.fillWidth: true
            RowLayout {
                spacing: 20
                Repeater {
                    Layout.fillWidth: true
                    model: Library.favourites
                    delegate: ColumnLayout {
                        id: delegateItem
                        required property string title
                        required property string artists
                        required property string videoId

                        Layout.fillWidth: false
                        Layout.maximumWidth: 200
                        Kirigami.Card {
                            id: card
                            Layout.preferredHeight: 200

                            onClicked: play(delegateItem.videoId)

                            ThumbnailSource {
                                id: thumbnailSource
                                videoId: delegateItem.videoId
                            }

                            header: Image {
                                sourceSize: "200x200"
                                source: thumbnailSource.cachedPath
                                fillMode: Image.PreserveAspectCrop
                                asynchronous: true
                            }

                            actions: [
                                Kirigami.Action {
                                    icon.name: "delete"
                                    onTriggered: Library.removeFavourite(delegateItem.videoId)
                                }

                            ]
                        }
                        Item {
                            height: 5
                        }
                        Controls.Label {
                            Layout.maximumWidth: 200
                            text: delegateItem.title
                            elide: Qt.ElideRight
                        }
                        Item {
                            height: 5
                        }
                    }
                }
            }
        }
        Item {
            height: 20
        }

        RowLayout {
            Layout.fillWidth: true
            Kirigami.Heading {
                text: i18n("Most played")
                Layout.alignment: Qt.AlignLeft
            }

            // Spacer
            Item {
                Layout.fillWidth: true
            }

            Controls.ToolButton {
                text: i18n("Show All")
                Layout.alignment: Qt.AlignRight
                icon.name: "arrow-right"
                onClicked: pageStack.layers.push("qrc:/PlaybackHistory.qml", {
                                                     "dataModel": Library.playbackHistory,
                                                     "title": i18n("Played Songs")
                                                 })
            }
        }
        Controls.ScrollView {
            Layout.fillWidth: true
            RowLayout {
                spacing: 20

                Repeater {
                    Layout.fillWidth: true
                    model: Library.mostPlayed
                    delegate: ColumnLayout {
                        id: mpdelegateItem
                        required property string title
                        required property string artists
                        required property string videoId

                        Layout.fillWidth: false
                        Layout.maximumWidth: 200
                        Kirigami.Card {
                            id: mpcard
                            Layout.preferredHeight: 200

                            onClicked: play(mpdelegateItem.videoId)

                            ThumbnailSource {
                                id: mpthumbnailSource
                                videoId: mpdelegateItem.videoId
                            }

                            header: Image {
                                sourceSize: "200x200"
                                source: mpthumbnailSource.cachedPath
                                fillMode: Image.PreserveAspectCrop
                                asynchronous: true
                            }
                        }
                        Item {
                            height: 5
                        }
                        Controls.Label {
                            Layout.maximumWidth: 200
                            text: mpdelegateItem.title
                            elide: Qt.ElideRight
                        }
                        Item {
                            height: 5
                        }
                    }
                }
            }
        }
    }
}
