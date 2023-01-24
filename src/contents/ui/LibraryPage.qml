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
    Kirigami.Theme.colorSet: Kirigami.Theme.View
    title: "AudioTube"

    rightPadding: 0
    leftPadding: 0

    SongMenu {
        id: menu
    }

    ColumnLayout {
        RowLayout {
            Layout.fillWidth: true
            Kirigami.Heading {
                text: i18n("Favourites")
                Layout.alignment: Qt.AlignLeft
                leftPadding: 15
            }

            // Spacer
            Item {
                Layout.fillWidth: true
            }

            Controls.ToolButton {
                text: i18n("Show All")
                Layout.alignment: Qt.AlignRight
                icon.name: "arrow-right"
                onClicked: {pageStack.push("qrc:/PlaybackHistory.qml", {
                      "dataModel": Library.favourites,
                      "title": i18n("Favourites"),
                      "objectName": "favourites"
                  })}
            }
        }

        Controls.ScrollView {
            leftPadding: 15
            rightPadding: 25
            Layout.fillWidth: true
            RowLayout {
                spacing: 20
                Repeater {
                    Layout.fillWidth: true
                    model: Library.favourites
                    delegate: ColumnLayout {
                        id: delegateItem
                        required property string title
                        required property var artists
                        required property string artistsDisplayString
                        required property string videoId
                        required property int index

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
                                        favTitle.color = Kirigami.Theme.hoverColor
                                        favSubtitle.color = Kirigami.Theme.hoverColor
                                        favTitle.font.bold = true
                                        playAnimationPosition.running = true
                                        playAnimationOpacity.running = true
                                    }

                                }

                                onExited:{
                                    favSelected.visible = false
                                    favTitle.color = Kirigami.Theme.textColor
                                    favSubtitle.color = Kirigami.Theme.disabledTextColor
                                    favTitle.font.bold = false
                                }
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
                                    id:favTitle
                                    text: delegateItem.title
                                    Layout.maximumWidth: 200
                                    Layout.fillWidth: true
                                    leftPadding: 5
                                    elide: Text.ElideRight

                                }
                                Controls.Label {
                                    id: favSubtitle
                                    Layout.fillWidth: true
                                    Layout.maximumWidth: 200
                                    leftPadding: 5
                                    color: Kirigami.Theme.disabledTextColor
                                    text: delegateItem.artistsDisplayString
                                    elide: Text.ElideRight
                                }
                            }
                            Controls.ToolButton {
                                Layout.fillHeight: true
                                icon.name: "view-more-horizontal-symbolic"
                                onPressed: menu.openForSong(delegateItem.videoId,
                                                               delegateItem.title,
                                                               delegateItem.artists,
                                                               delegateItem.artistsDisplayString)
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
                leftPadding: 15
            }

            // Spacer
            Item {
                Layout.fillWidth: true
            }

            Controls.ToolButton {
                text: i18n("Show All")
                Layout.alignment: Qt.AlignRight
                icon.name: "arrow-right"
                onClicked: {pageStack.push("qrc:/PlaybackHistory.qml", {
                      "dataModel": Library.playbackHistory,
                      "title": i18n("played Songs"),
                      "objectName": "history"
                  })}
            }
        }
        Controls.ScrollView {
            leftPadding: 15
            rightPadding: 25
            Layout.fillWidth: true
            RowLayout {
                spacing: 20
                Repeater {
                    Layout.fillWidth: true
                    model: Library.mostPlayed
                    delegate: ColumnLayout {
                        id: mpdelegateItem
                        required property string title
                        required property var artists
                        required property string artistsDisplayString
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
                                        recTitle.color = Kirigami.Theme.hoverColor
                                        recSubtitle.color = Kirigami.Theme.hoverColor
                                        recTitle.font.bold = true
                                        playAnimationPositionRec.running = true
                                        playAnimationOpacityRec.running = true
                                }
                                onExited:{
                                    recSelected.visible = false
                                    recTitle.color = Kirigami.Theme.textColor
                                    recSubtitle.color = Kirigami.Theme.disabledTextColor
                                    recTitle.font.bold = false
                                }

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
                                    id: recTitle
                                    text: mpdelegateItem.title
                                    Layout.maximumWidth: 200
                                    Layout.fillWidth: true
                                    leftPadding: 5
                                    elide: Text.ElideRight

                                }
                                Controls.Label {
                                    id: recSubtitle
                                    Layout.fillWidth: true
                                    Layout.maximumWidth: 200
                                    leftPadding: 5
                                    color: Kirigami.Theme.disabledTextColor
                                    text: mpdelegateItem.artistsDisplayString
                                    elide: Text.ElideRight
                                }
                            }
                            Controls.ToolButton {
                                Layout.fillHeight: true
                                icon.name: "view-more-horizontal-symbolic"
                                onPressed: menu.openForSong(mpdelegateItem.videoId,
                                                              mpdelegateItem.title,
                                                              mpdelegateItem.artists,
                                                              mpdelegateItem.artistsDisplayString)
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
