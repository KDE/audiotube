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
    
    required property int progressBarHeight
    required property int minimizedPlayerContentHeight
    
    required property var info // VideoInfoExtractor object
    required property var audio // Audio object
    
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

                    Image {
                        id: mainIcon
                        anchors.fill: parent
                        asynchronous: true
                        mipmap: true

                        source: info.thumbnail

                        sourceSize.width: 512
                        sourceSize.height: 512
                        fillMode: Image.PreserveAspectFit
                    }
                }
                
                // track information
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    
                    Label {
                        id: mainLabel
                        text: info.title ? info.title : i18n("No media playing")
                        textFormat: Text.PlainText
                        wrapMode: Text.Wrap
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignLeft
                        elide: Text.ElideRight
                        maximumLineCount: 1
                        // Hardcoded because the footerbar blur always makes a dark-ish
                        // background, so we don't want to use a color scheme color that
                        // might also be dark
                        color: "white"
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
                        // Hardcoded because the footerbar blur always makes a dark-ish
                        // background, so we don't want to use a color scheme color that
                        // might also be dark
                        color: "white"
                    }
                }
            }
            
            MouseArea {
                id: trackClick
                anchors.fill: parent
                hoverEnabled: true
                onClicked: root.requestOpen()
            }
        }
        
        ToolButton {
            id: playPauseButton
            Layout.preferredHeight: Kirigami.Units.gridUnit * 3
            Layout.maximumWidth: height
            Layout.preferredWidth: height
            
            enabled: info.audioUrl != ""
            onClicked: audio.playbackState === Audio.PlayingState ? audio.pause() : audio.play()
            
            icon.name: audio.playbackState === Audio.PlayingState ? "media-playback-pause" : "media-playback-start"
            icon.width: Kirigami.Units.gridUnit
            icon.height: Kirigami.Units.gridUnit
            icon.color: "white"
            
            Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
            Kirigami.Theme.inherit: false
        }
        
        ToolButton {
            id: skipForwardButton
            Layout.rightMargin: Math.floor(Kirigami.Units.smallSpacing / 2)
            Layout.preferredHeight: Kirigami.Units.gridUnit * 3
            Layout.maximumWidth: height
            Layout.preferredWidth: height
            
            enabled: UserPlaylistModel.canSkip
            onClicked: UserPlaylistModel.next()
            
            icon.name: "media-skip-forward"
            icon.width: Kirigami.Units.gridUnit
            icon.height: Kirigami.Units.gridUnit
            icon.color: "white"
            
            Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
            Kirigami.Theme.inherit: false
        }
    }
}
