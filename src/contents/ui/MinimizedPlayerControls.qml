// SPDX-FileCopyrightText: 2020-2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import QtGraphicalEffects 1.0
import QtMultimedia 5.12

import org.kde.kirigami 2.19 as Kirigami
import org.kde.ytmusic 1.0

Item {
    id: root
    z: 2000
    
    required property int progressBarHeight
    required property int minimizedPlayerContentHeight
    
    required property var info // VideoInfoExtractor object
    required property var audio // Audio object
    
    readonly property bool isWidescreen: width >= Kirigami.Units.gridUnit * 50
    signal requestOpen()
    
    Rectangle {
        id: miniProgressBar
        z: 1
        anchors.top: parent.top
        anchors.left: parent.left
        height: root.progressBarHeight
        color: Kirigami.Theme.highlightColor
        width: parent.width * (audio.position / audio.duration)
    }
    
    RowLayout {
        width: !isWidescreen ? root.width - controlButtonBox.width: root.width
        anchors.fill: parent
        spacing: 0
        
        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true

            color: Qt.rgba(0, 0, 0, trackClick.containsMouse ? 0.1 : trackClick.pressed ? 0.3 : 0)

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: Kirigami.Units.largeSpacing
                spacing: Kirigami.Units.largeSpacing
                
                // track image
                Item {
                    property double imageSize: root.height - Kirigami.Units.largeSpacing * 2
                    
                    Layout.alignment: Qt.AlignVCenter
                    Layout.maximumWidth: imageSize
                    Layout.preferredWidth: imageSize
                    Layout.maximumHeight: imageSize
                    Layout.minimumHeight: imageSize

                    RoundedImage {
                        id: mainIcon
                        anchors.fill: parent
                        visible: !loadingIndicator.visible
                        source: info.thumbnail
                        radius: 5
                    }
                    
                    BusyIndicator {
                        id: loadingIndicator
                        anchors.centerIn: parent
                        visible: UserPlaylistModel.loading
                        Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                        Kirigami.Theme.inherit: false
                    }

                }
                
                // track information
                ColumnLayout {
                    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    
                    Label {
                        id: mainLabel
                        text: info.title ? info.title : i18n("No media playing")
                        textFormat: Text.PlainText
                        wrapMode: Text.Wrap
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                        Layout.fillWidth: true
                        Layout.maximumWidth: Kirigami.Units.gridUnit * 15
                        horizontalAlignment: Text.AlignLeft
                        elide: Text.ElideRight
                        maximumLineCount: 1
                        // Hardcoded because the footerbar blur always makes a dark-ish
                        // background, so we don't want to use a color scheme color that
                        // might also be dark
                        color: Kirigami.Theme.textColor
                        font.weight: Font.Bold
                    }
                    
                    Label {
                        id: authorLabel
                        text: info.artist ? info.artist : info.channel
                        textFormat: Text.PlainText
                        wrapMode: Text.Wrap
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignLeft
                        elide: Text.ElideRight
                        maximumLineCount: 1
                        Layout.maximumWidth: Kirigami.Units.gridUnit * 15
                        // Hardcoded because the footerbar blur always makes a dark-ish
                        // background, so we don't want to use a color scheme color that
                        // might also be dark
                        color: Kirigami.Theme.disabledTextColor
                    }
                }
                Item{Layout.fillWidth: true}

            }
            MouseArea {
                id: trackClick
                anchors.fill: parent
                hoverEnabled: true
                onClicked: root.requestOpen()
            }
        }
    }

    RowLayout {
        id: controlButtonBox
        anchors.centerIn: isWidescreen ? parent : null
        anchors.right: !isWidescreen ? parent.right : null
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        spacing: 2

        Label {
            color: "white"
            visible: info.title && root.isWidescreen
            text: PlayerUtils.formatTimestamp(audio.position)
            Layout.rightMargin: 20
        }
        
        Button {
            id: skipBackwardButton
            implicitHeight: 40
            implicitWidth: 40
            Layout.fillHeight: !isWidescreen

            Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
            Kirigami.Theme.inherit: false

            enabled: UserPlaylistModel.canSkipBack
            onClicked: UserPlaylistModel.previous()
            
            contentItem: Item {
                Kirigami.Icon {
                    anchors.centerIn: parent
                    source:"media-skip-backward"
                    color: "white"
                    width: Kirigami.Units.gridUnit
                    height: Kirigami.Units.gridUnit
                }
            }
            
            background: Kirigami.ShadowedRectangle {
                corners.topLeftRadius: isWidescreen ? 7 : 0
                corners.bottomLeftRadius: isWidescreen ? 7 : 0

                color: if (isWidescreen) {
                    if (parent.down) {
                        Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.3)
                    } else if(parent.hovered) {
                        Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.7)
                    } else {
                        Qt.rgba(1, 1, 1, 0.2)
                    }
                } else {
                    if (parent.down) {
                        Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.3)
                    } else if (parent.hovered){
                        Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.7)
                    } else {
                        "transparent"
                    }
                }
            }
        }

        Button {
            id: playPauseButton
            implicitHeight: 40
            implicitWidth: 60
            Layout.fillHeight: !isWidescreen
            
            onClicked: audio.playbackState === Audio.PlayingState ? audio.pause() : audio.play()
            contentItem: Item {
                Kirigami.Icon {
                    anchors.centerIn:parent
                    source: audio.playbackState === Audio.PlayingState ? "media-playback-pause" : "media-playback-start"
                    color: "white"
                    width: Kirigami.Units.gridUnit
                    height: Kirigami.Units.gridUnit
                }
            }
            
            background: Kirigami.ShadowedRectangle {
                color: if (isWidescreen) {
                    if (parent.down) {
                        Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.3)
                    } else if (parent.hovered) {
                        Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.7)
                    } else {
                        Qt.rgba(1, 1, 1, 0.2)
                    }
                } else {
                    if (parent.down) {
                        Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.3)
                    } else if (parent.hovered) {
                        Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.7)
                    } else {
                        "transparent"
                    }
                }

            }

        }

        Button {
            id: skipForwardButton
            implicitHeight: 40
            implicitWidth: 40
            Layout.fillHeight: !isWidescreen
            Layout.rightMargin: isWidescreen ? 0 : 10
            Layout.alignment: !Qt.AlignVCenter

            Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
            Kirigami.Theme.inherit: false

            enabled: UserPlaylistModel.canSkip
            onClicked: UserPlaylistModel.next()
            contentItem: Item {
                Kirigami.Icon {
                    anchors.centerIn: parent
                    source:"media-skip-forward"
                    color: "white"
                    width: Kirigami.Units.gridUnit
                    height: Kirigami.Units.gridUnit
                }
            }
            background: Kirigami.ShadowedRectangle {
                corners.topRightRadius: isWidescreen ? 7 : 0
                corners.bottomRightRadius: isWidescreen ? 7 : 0
                color: if (isWidescreen) {
                    if (parent.down) {
                        Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.3)
                    } else if (parent.hovered) {
                        Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.7)
                    } else {
                        Qt.rgba(1, 1, 1, 0.2)
                    }
                } else {
                    if (parent.down) {
                        Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.3)
                    } else if(parent.hovered) {
                        Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.7)
                    } else {
                        "transparent"
                    }
                }
            }
        }

        Label {
            color: "white"
            visible: info.title && root.isWidescreen
            text: PlayerUtils.formatTimestamp(audio.duration)
            Layout.leftMargin: 20
        }
    }
}
