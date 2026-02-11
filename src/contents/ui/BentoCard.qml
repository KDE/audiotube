// SPDX-FileCopyrightText: 2026 Tanveer Ahmed Mansuri
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects
import QtQuick.Templates as T

T.ItemDelegate {
    id: root
    
    required property int index
    required property string title
    required property string artistsDisplayString
    required property string thumbnailUrl

    property bool wide: false
    property bool tall: false
    
    // Song-specific properties for menu
    required property string videoId
    required property var artists
    readonly property bool isSong: videoId !== ""

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)
    
    hoverEnabled: true

    // Z-index elevation to prevent flickering (overlapping neighbors stealing hover)
    z: root.hovered ? 2 : 0

    // Layout properties for Bento Grid
    Layout.fillWidth: true
    Layout.fillHeight: true
    Layout.rowSpan: tall ? 2 : 1
    Layout.columnSpan: wide ? 2 : 1
    Layout.preferredHeight: 180 * (tall ? 2.1 : 1.0)

    background: Kirigami.ShadowedRectangle {
        implicitWidth: 180 * (wide ? 2 : 1)
        implicitHeight: 180 * (tall ? 2 : 1)

        radius: 12
        color: Qt.rgba(0.12, 0.12, 0.12, 0.7) // Glassmorphic semi-transparent background
        
        // Scale animation on hover and press
        scale: root.pressed ? 0.98 : (root.hovered ? 1.02 : 1.0)
        Behavior on scale { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
        
        // Softer, deeper shadows
        shadow.size: root.hovered ? 30 : 15
        shadow.yOffset: root.hovered ? 8 : 4
        shadow.color: Qt.rgba(0,0,0, root.hovered ? 0.4 : 0.2)
        
        Behavior on shadow.size { NumberAnimation { duration: 200 } }
        Behavior on shadow.yOffset { NumberAnimation { duration: 200 } }

        // Fallback Gradient for items without image (e.g. Moods)
        Rectangle {
             anchors.fill: parent
             radius: 12
             visible: root.thumbnailUrl === "" || img.status === Image.Error
             gradient: Gradient {
                 GradientStop { position: 0.0; color: Qt.hsla((root.title.length * 5 % 360) / 360.0, 0.6, 0.5, 1.0) } // Dynamic hue based on title
                 GradientStop { position: 1.0; color: Qt.hsla(((root.title.length * 5 + 40) % 360) / 360.0, 0.7, 0.2, 1.0) }
             }
        }

        Image {
            id: img
            anchors.fill: parent
            source: root.thumbnailUrl
            fillMode: Image.PreserveAspectCrop
            visible: root.thumbnailUrl !== "" && status === Image.Ready
            
            onSourceChanged: {
                if (root.thumbnailUrl) console.log("BentoCard: Image source changed to:", root.thumbnailUrl)
            }
            
            layer.enabled: true
            layer.effect: OpacityMask {
                maskSource: Rectangle { 
                    width: img.width
                    height: img.height
                    radius: 12
                    visible: false
                }
            }
        }
        
        // Gradient overlay for text readability
        Rectangle {
            anchors.fill: parent
            radius: 12
            gradient: Gradient {
                GradientStop { position: 0.0; color: "transparent" }
                GradientStop { position: 0.5; color: "transparent" }
                GradientStop { position: 1.0; color: Qt.rgba(0,0,0,0.9) }
            }
        }

        // Highlight border on hover
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: Kirigami.Theme.highlightColor
            border.width: 2
            radius: 12
            opacity: root.hovered ? 0.5 : 0
            Behavior on opacity { NumberAnimation { duration: 200 } }
        }
    }
    
    contentItem: Item {
        // Play Button Overlay (Fades in on hover)
        Rectangle {
            anchors.centerIn: parent
            width: 48
            height: 48
            radius: 24
            color: Qt.rgba(0, 0, 0, 0.6)
            visible: root.hovered
            opacity: root.hovered ? 1.0 : 0.0
            Behavior on opacity { NumberAnimation { duration: 200 } }
            
            Kirigami.Icon {
                anchors.centerIn: parent
                source: "media-playback-start"
                width: 24
                height: 24
                color: "white"
            }
        }
        
        // Overflow menu button (top-right, visible on hover for songs)
        Controls.RoundButton {
            id: overflowButton
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: 8
            visible: root.isSong && (root.hovered || hovered)
            opacity: (root.hovered || hovered) ? 1 : 0
            
            Behavior on opacity {
                NumberAnimation { duration: 150 }
            }
            icon.name: "overflow-menu-symbolic"
            
            onClicked: root.Controls.ContextMenu.requested(Qt.point(0, 0))
            
            Controls.ToolTip.visible: hovered
            Controls.ToolTip.text: i18n("More options")
        }

        ColumnLayout {
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
                margins: Kirigami.Units.gridUnit
            }
            spacing: Kirigami.Units.smallSpacing
            
            Controls.Label {
                text: root.title
                font.weight: Font.Bold
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * (root.wide || root.tall ? 1.4 : 1.1)
                color: "white"
                elide: Text.ElideRight
                Layout.fillWidth: true
                maximumLineCount: 2
            }
            
            Controls.Label {
                text: root.artistsDisplayString
                visible: text !== ""
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 0.9
                color: Qt.rgba(1,1,1,0.8)
                elide: Text.ElideRight
                Layout.fillWidth: true
                maximumLineCount: 1
            }
        }
    }
   
}
