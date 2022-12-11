// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.1
import QtQuick.Controls 2.12 as Controls
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.14 as Kirigami

import org.kde.ytmusic 1.0

Kirigami.ScrollablePage {
    objectName: "libraryPage"

    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.None

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

                        Kirigami.ShadowedRectangle {
                            color: Kirigami.Theme.backgroundColor
                            id: favCover
                            MouseArea {
                                id: favArea
                                anchors.fill: parent
                                onClicked: play(delegateItem.videoId)
                                hoverEnabled: !Kirigami.Settings.hasTransientTouchInput
                                onEntered: {
                                    if (!Kirigami.Settings.hasTransientTouchInput){
                                        favSelected.visible = true
                                        playAnimationPosition.running = true
                                        playAnimationOpacity.running = true
                                    }

                                }

                                onExited: favSelected.visible = false

                            }
                            Layout.margins: 5

                            width: 200
                            height: 200
                            radius: 10
                            shadow.size: 15
                            shadow.xOffset: 5
                            shadow.yOffset: 5
                            shadow.color: Qt.rgba(0, 0, 0, 0.2)
                            ThumbnailSource {
                                id: thumbnailSource
                                videoId: delegateItem.videoId
                            }
                            RoundedImage {
                                source: thumbnailSource.cachedPath
                                height: 200
                                width: height
                                radius: 10
                            }
                            Rectangle {
                                id: favSelected

                                Rectangle {
                                    anchors.fill: parent
                                    color: Kirigami.Theme.hoverColor
                                    radius: 10
                                    opacity: 0.2
                                }
                                Item{
                                    height: parent.height
                                    width: parent.width
                                    NumberAnimation on opacity{
                                        id: playAnimationOpacity
                                        easing.type: Easing.OutCubic
                                        running: false
                                        from: 0; to: 1
                                    }
                                    NumberAnimation on y {
                                        id: playAnimationPosition
                                        easing.type: Easing.OutCubic
                                        running: false
                                        from: 20; to: 0
                                        duration: 100
                                    }
                                    Rectangle {
                                        height: 45
                                        width: 45
                                        radius: 50
                                        color: Kirigami.Theme.hoverColor
                                        opacity: 0.8
                                        anchors.centerIn: parent


                                    }
                                    Kirigami.Icon {
                                        x: 100 - 0.43 * height
                                        y: 100 - 0.5  * height
                                        color: "white"
                                        source: "media-playback-start"
                                    }
                                }
                                visible: false
                                anchors.fill: parent

                                radius: 9

                                border.color: Kirigami.Theme.hoverColor
                                border.width: 2
                                color: "transparent"
                            }
                        }

                        RowLayout {
                            ColumnLayout {
                                Controls.Label {
                                    text: delegateItem.title
                                    Layout.maximumWidth: 200
                                    Layout.fillWidth: true
                                    leftPadding: 5
                                    elide: Text.ElideRight

                                }
                                Controls.Label {
                                    Layout.fillWidth: true
                                    Layout.maximumWidth: 200
                                    leftPadding: 5
                                    color: Kirigami.Theme.disabledTextColor
                                    text: delegateItem.artists
                                    elide: Text.ElideRight
                                }
                            }
                            Controls.ToolButton {
                                Layout.fillHeight: true
                                icon.name: "view-more-horizontal-symbolic"
                                onPressed: favMenu.openForSong(delegateItem.videoId, delegateItem.title, delegateItem.artists, delegateItem.artists)
                                SongMenu {
                                    id:favMenu
                                }
                            }

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

                        Kirigami.ShadowedRectangle {
                            color: Kirigami.Theme.backgroundColor
                            id: recCover
                            MouseArea {
                                id: recArea
                                anchors.fill: parent
                                onClicked: play(mpdelegateItem.videoId)
                                hoverEnabled: !Kirigami.Settings.hasTransientTouchInput
                                onEntered: {
                                    if (!Kirigami.Settings.hasTransientTouchInput)
                                        recSelected.visible = true
                                        playAnimationPositionRec.running = true
                                        playAnimationOpacityRec.running = true
                                }
                                onExited: recSelected.visible = false

                            }
                            Layout.margins: 5

                            width: 200
                            height: 200
                            radius: 10
                            shadow.size: 15
                            shadow.xOffset: 5
                            shadow.yOffset: 5
                            shadow.color: Qt.rgba(0, 0, 0, 0.2)
                            ThumbnailSource {
                                id: mpthumbnailSource
                                videoId: mpdelegateItem.videoId
                            }
                            RoundedImage {
                                source: mpthumbnailSource.cachedPath
                                height: 200
                                width: height
                                radius: 10
                            }
                            Rectangle {
                                id: recSelected

                                Rectangle {
                                    anchors.fill: parent
                                    color: Kirigami.Theme.hoverColor
                                    radius: 10
                                    opacity: 0.2
                                }
                                Item{
                                    height: parent.height
                                    width: parent.width
                                    NumberAnimation on opacity{
                                        id: playAnimationOpacityRec
                                        easing.type: Easing.OutCubic
                                        running: false
                                        from: 0; to: 1
                                    }
                                    NumberAnimation on y {
                                        id: playAnimationPositionRec
                                        easing.type: Easing.OutCubic
                                        running: false
                                        from: 20; to: 0
                                        duration: 100
                                    }
                                    Rectangle {
                                        height: 45
                                        width: 45
                                        radius: 50
                                        color: Kirigami.Theme.hoverColor
                                        opacity: 0.8
                                        anchors.centerIn: parent


                                    }
                                    Kirigami.Icon {
                                        x: 100 - 0.43 * height
                                        y: 100 - 0.5  * height
                                        color: "white"
                                        source: "media-playback-start"
                                    }
                                }

                                visible: false
                                anchors.fill: parent

                                radius: 9

                                border.color: Kirigami.Theme.hoverColor
                                border.width: 2
                                color: "transparent"
                            }
                        }
                        RowLayout {
                            ColumnLayout {
                                Controls.Label {
                                    text: mpdelegateItem.title
                                    Layout.maximumWidth: 200
                                    Layout.fillWidth: true
                                    leftPadding: 5
                                    elide: Text.ElideRight

                                }
                                Controls.Label {
                                    Layout.fillWidth: true
                                    Layout.maximumWidth: 200
                                    leftPadding: 5
                                    color: Kirigami.Theme.disabledTextColor
                                    text: mpdelegateItem.artists
                                    elide: Text.ElideRight
                                }
                            }
                            Controls.ToolButton {
                                Layout.fillHeight: true
                                icon.name: "view-more-horizontal-symbolic"
                                onPressed: mpMenu.openForSong(mpdelegateItem.videoId, mpdelegateItem.title, mpdelegateItem.artists, mpdelegateItem.artists)
                                SongMenu{
                                    id:mpMenu
                                }
                            }

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
