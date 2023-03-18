// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.1
import QtQuick.Controls 2.12 as Controls
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.19 as Kirigami

import org.kde.ytmusic 1.0

import ".."


Controls.Button {
    id: root
    property Kirigami.Action singleAction

    property int margin: 10

    height: 55
    width: 55
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
        shadow.size: 10
        shadow.xOffset: 2
        shadow.yOffset: 2
        shadow.color: Qt.rgba(0, 0, 0, 0.2)
        Behavior on color {
                enabled: highlightBackground
                ColorAnimation {
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.OutCubic
                }
            }
        Behavior on border.color {
            enabled: highlightBorder
            ColorAnimation {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.OutCubic
            }
        }
        radius: 10
    }
    contentItem: Item{
        Kirigami.Icon{
            implicitHeight: Kirigami.Units.gridUnit * 1.2
            source: root.singleAction.icon.name
            anchors.centerIn: parent
        }
    }
    text: root.singleAction.text
    onClicked: root.singleAction.triggered()
}
