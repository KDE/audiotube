// SPDX-FileCopyrightText: 2026 Tanveer Ahmed Mansuri
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Templates as T
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects
import org.kde.audiotube

T.ItemDelegate {
    id: root

    required property string title
    required property string artistsDisplayString
    required property string thumbnailUrl
    
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    Layout.fillWidth: true
    Layout.fillHeight: true
    
    signal playClicked()
    signal openClicked()

    leftPadding: Kirigami.Units.gridUnit
    rightPadding: Kirigami.Units.gridUnit
    topPadding: Kirigami.Units.gridUnit
    bottomPadding: Kirigami.Units.gridUnit
    
    // Glass backdrop (matching AlbumCoverItem)
    background: Kirigami.ShadowedRectangle {
        anchors.fill: parent
        radius: 12

        color: Qt.rgba(Kirigami.Theme.backgroundColor.r, Kirigami.Theme.backgroundColor.g, Kirigami.Theme.backgroundColor.b, 0.4)
        border.color: Qt.rgba(1, 1, 1, 0.15)
        border.width: 1

        shadow.size: 15
        shadow.xOffset: 0
        shadow.yOffset: 0
        shadow.color: Qt.rgba(0, 0, 0, 0.1)

        // Background Container with Rounded Corners (Masked Content)
        Item {
            id: bgContainer
            anchors.fill: parent
            
            layer.enabled: true
            layer.effect: OpacityMask {
                maskSource: Rectangle {
                    width: bgContainer.width
                    height: bgContainer.height
                    radius: 16
                    visible: false
                }
            }

            // Background Art
            Image {
                id: bgImage
                anchors.fill: parent
                source: root.thumbnailUrl
                fillMode: Image.PreserveAspectCrop
                visible: false 
            }
            
            FastBlur {
                anchors.fill: bgImage
                source: bgImage
                radius: 64
                cached: true
                transparentBorder: false
            }

            // Gradient Overlay
            Rectangle {
                anchors.fill: parent
                gradient: Gradient {
                    GradientStop { position: 0.0; color: Qt.rgba(0,0,0,0.3) }
                    GradientStop { position: 1.0; color: Qt.rgba(0,0,0,0.8) }
                }
            }
        }
    }

    contentItem: RowLayout {
        spacing: Kirigami.Units.gridUnit

        // Main Album Art (Sharp) - Vertically Centered
        Kirigami.ShadowedRectangle {
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: Kirigami.Units.gridUnit * 10
            Layout.preferredHeight: Kirigami.Units.gridUnit * 10
            radius: Kirigami.Units.cornerRadius
            shadow.size: 20
            shadow.color: Qt.rgba(0,0,0,0.4)
            
            Image {
                anchors.fill: parent
                source: root.thumbnailUrl
                fillMode: Image.PreserveAspectCrop
                layer.enabled: true
                layer.effect: OpacityMask {
                    maskSource: Rectangle { 
                        width: Kirigami.Units.gridUnit * 10
                        height: Kirigami.Units.gridUnit * 10
                        radius: Kirigami.Units.cornerRadius
                        visible: false 
                    }
                }
            }
        }
        
        ColumnLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true

            spacing: 0
            
            Kirigami.Heading {
                text: i18n("Featured Pick")
                font.capitalization: Font.AllUppercase
                font.letterSpacing: 4
                color: Kirigami.Theme.highlightColor
                font.weight: Font.Black
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.4
                Layout.fillWidth: true
            }
            
            ColumnLayout {
                spacing: Kirigami.Units.smallSpacing
                
                Controls.Label {
                    text: root.title
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize * 2.8
                    font.weight: Font.ExtraBold
                    color: "white"
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                    maximumLineCount: 2 
                    Layout.bottomMargin: -4 
                }

                Controls.Label {
                    text: root.artistsDisplayString
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.2
                    color: Qt.rgba(1,1,1,0.7)
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                    font.weight: Font.Medium
                }
            }

            Item { Layout.fillHeight: true }

            Controls.Button {
                id: playBtn

                icon.name: "media-playback-start"
                text: i18n("Play Now")
                
                onClicked: root.playClicked()
            }
        }
    }
}
