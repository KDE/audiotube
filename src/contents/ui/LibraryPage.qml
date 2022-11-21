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

                        Kirigami.ShadowedRectangle {
                            id: favCover
                            MouseArea {
                                id: favArea
                                anchors.fill: parent
                                onClicked: play(delegateItem.videoId)
                                hoverEnabled: !Kirigami.Settings.hasTransientTouchInput
                                onEntered: {
                                    if (!Kirigami.Settings.hasTransientTouchInput)
                                        favSelected.visible = true
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
                            Rectangle {
                                width: 200
                                height: 200

                                color: "transparent"

                                //this Rectangle is needed to keep the source image's fillMode
                                ThumbnailSource {
                                    id: thumbnailSource
                                    videoId: delegateItem.videoId
                                }
                                Rectangle {

                                    id: imageSource

                                    anchors.fill: parent
                                    Image {
                                        anchors.fill: parent
                                        source: thumbnailSource.cachedPath
                                        fillMode: Image.PreserveAspectCrop
                                        asynchronous: true
                                    }
                                    visible: false

                                    layer.enabled: true
                                }

                                Rectangle {
                                    anchors.fill: parent
                                    radius: 10
                                    layer.enabled: true
                                    layer.samplerName: "maskSource"
                                    layer.effect: ShaderEffect {

                                        property var colorSource: imageSource
                                        fragmentShader: "
                                            uniform lowp sampler2D colorSource;
                                            uniform lowp sampler2D maskSource;
                                            uniform lowp float qt_Opacity;
                                            varying highp vec2 qt_TexCoord0;
                                            void main() {
                                                gl_FragColor = texture2D(colorSource, qt_TexCoord0) * texture2D(maskSource, qt_TexCoord0).a * qt_Opacity;
                                            }"
                                    }
                                }

                                Controls.ToolButton {
                                    anchors.right: parent.right
                                    anchors.bottom: parent.bottom
                                    icon.name: "delete"
                                    onClicked: Library.removeFavourite(
                                                   delegateItem.videoId)
                                    z: 2000
                                }
                                Rectangle {
                                    id: favSelected

                                    Rectangle {
                                        anchors.fill: parent
                                        color: Kirigami.Theme.hoverColor
                                        radius: 10
                                        opacity: 0.2
                                    }

                                    visible: false
                                    anchors.fill: parent
                                    radius: 9
                                    border.color: Kirigami.Theme.hoverColor
                                    border.width: 2
                                    color: "transparent"
                                }
                            }
                        }
                        Controls.Label {
                            Layout.maximumWidth: 200
                            leftPadding:5

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

                        Kirigami.ShadowedRectangle {
                            id: recCover
                            MouseArea {
                                id: recArea
                                anchors.fill: parent
                                onClicked: play(mpdelegateItem.videoId)
                                hoverEnabled: !Kirigami.Settings.hasTransientTouchInput
                                onEntered: {
                                    if (!Kirigami.Settings.hasTransientTouchInput)
                                        recSelected.visible = true
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
                            Rectangle {
                                width: 200
                                height: 200

                                color: "transparent"

                                //this Rectangle is needed to keep the source image's fillMode

                                ThumbnailSource {
                                    id: mpthumbnailSource
                                    videoId: mpdelegateItem.videoId
                                }
                                Rectangle {

                                    id: recImageSource

                                    anchors.fill: parent
                                    Image {
                                        anchors.fill: parent
                                        source: mpthumbnailSource.cachedPath
                                        fillMode: Image.PreserveAspectCrop
                                        asynchronous: true
                                    }
                                    visible: false

                                    layer.enabled: true
                                }

                                Rectangle {
                                    anchors.fill: parent
                                    radius: 10
                                    layer.enabled: true
                                    layer.samplerName: "maskSource"
                                    layer.effect: ShaderEffect {

                                        property var colorSource: recImageSource
                                        fragmentShader: "
                                            uniform lowp sampler2D colorSource;
                                            uniform lowp sampler2D maskSource;
                                            uniform lowp float qt_Opacity;
                                            varying highp vec2 qt_TexCoord0;
                                            void main() {
                                                gl_FragColor = texture2D(colorSource, qt_TexCoord0) * texture2D(maskSource, qt_TexCoord0).a * qt_Opacity;
                                            }"
                                    }
                                }


                                Rectangle {
                                    id: recSelected

                                    Rectangle {
                                        anchors.fill: parent
                                        color: Kirigami.Theme.hoverColor
                                        radius: 10
                                        opacity: 0.2
                                    }

                                    visible: false
                                    anchors.fill: parent

                                    radius: 9

                                    border.color: Kirigami.Theme.hoverColor
                                    border.width: 2
                                    color: "transparent"
                                }
                            }
                        }
                        Controls.Label {
                            Layout.maximumWidth: 200
                            leftPadding:5
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
