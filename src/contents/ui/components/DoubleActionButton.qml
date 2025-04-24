// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

import org.kde.audiotube

Kirigami.ShadowedRectangle{
    id: root
    property Kirigami.Action leftAction
    property Kirigami.Action rightAction


    property int margin: 10
    radius: 10
    color: "transparent"
    height: 55
    width: 2* height - 1
    shadow.size: 10
    shadow.xOffset: 2
    shadow.yOffset: 2
    shadow.color: Qt.rgba(0, 0, 0, 0.2)
    Controls.Button {
        id: rightButton
        property int margin: 10
        background: Kirigami.ShadowedRectangle{
            Kirigami.Theme.inherit: false
            Kirigami.Theme.colorSet: Kirigami.Theme.Window
            border.width: 1
            border.color: if (parent.down){
                    Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.hoverColor, Kirigami.Theme.backgroundColor, 0.4)
                }else if(parent.hovered){
                    Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.hoverColor, Kirigami.Theme.backgroundColor, 0.6)
                }else{
                    Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.backgroundColor, Kirigami.Theme.textColor, 0.2)
                }
            color: if (parent.down){
                    Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.hoverColor, Kirigami.Theme.backgroundColor, 0.6)
                }else if(parent.hovered){
                    Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.hoverColor, Kirigami.Theme.backgroundColor, 0.8)
                }else{
                    Kirigami.Theme.backgroundColor
                }
            Behavior on color {
                    enabled: true
                    ColorAnimation {
                        duration: Kirigami.Units.longDuration
                        easing.type: Easing.OutCubic
                    }
                }
            Behavior on border.color {
                enabled: true
                ColorAnimation {
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.OutCubic
                }
            }
            corners.topRightRadius: 10
            corners.bottomRightRadius: 10

        }
        contentItem: Item{
            Kirigami.Icon{
                implicitHeight: Kirigami.Units.gridUnit *1.2
                source: root.rightAction.icon.name
                anchors.centerIn: parent
            }
        }
        text: root.rightAction.text
        onClicked: root.rightAction.triggered()
        anchors.right: root.right
        height: 55
        width: 55
        display: Controls.AbstractButton.IconOnly
    }
    Controls.Button {
        id: leftButton
        background: Kirigami.ShadowedRectangle{
            Kirigami.Theme.inherit: false
            Kirigami.Theme.colorSet: Kirigami.Theme.Window
            border.width: 1
            border.color: if (parent.down){
                    Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.hoverColor, Kirigami.Theme.backgroundColor, 0.4)
                }else if(parent.hovered){
                    Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.hoverColor, Kirigami.Theme.backgroundColor, 0.6)
                }else{
                    Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.backgroundColor, Kirigami.Theme.textColor, 0.2)
                }
            color: if (parent.down){
                    Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.hoverColor, Kirigami.Theme.backgroundColor, 0.6)
                }else if(parent.hovered){
                    Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.hoverColor, Kirigami.Theme.backgroundColor, 0.8)
                }else{
                    Kirigami.Theme.backgroundColor
                }
            Behavior on color {
                    enabled: true
                    ColorAnimation {
                        duration: Kirigami.Units.longDuration
                        easing.type: Easing.OutCubic
                    }
                }
            Behavior on border.color {
                enabled: true
                ColorAnimation {
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.OutCubic
                }
            }
            corners.topLeftRadius: 10
            corners.bottomLeftRadius: 10
        }
        contentItem: Item{
            Kirigami.Icon{
                implicitHeight: Kirigami.Units.gridUnit * 1.2
                source: root.leftAction.icon.name
                anchors.centerIn: parent
            }
        }
        text: root.leftAction.text
        onClicked: root.leftAction.triggered()
        anchors.left: root.left
        height: 55
        width: 55
        display: Controls.AbstractButton.IconOnly
    }
}
