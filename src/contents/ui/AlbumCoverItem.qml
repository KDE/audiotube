// SPDX-FileCopyrightText: 2023 Jonah Brüchert <jbb@kaidan.im>
// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import QtGraphicalEffects 1.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.15 as Kirigami

ColumnLayout {
    id: root
    property alias contentItem: content.children
    property alias title: favTitle.text
    property alias subtitle: favSubtitle.text
    property bool showIcon: true

    signal clicked
    signal optionsClicked

    Kirigami.ShadowedRectangle {
        color: Kirigami.Theme.backgroundColor
        id: cover

        Layout.margins: 5

        MouseArea {
            id: coverArea
            anchors.fill: parent
            onClicked: root.clicked()
            hoverEnabled: !Kirigami.Settings.hasTransientTouchInput
            onEntered: {
                if (!Kirigami.Settings.hasTransientTouchInput) {
                    selection.visible = true
                    favTitle.color = Kirigami.Theme.hoverColor
                    favSubtitle.color = Kirigami.Theme.hoverColor
                    favTitle.font.bold = true
                    playAnimationPosition.running = true
                    playAnimationOpacity.running = true
                }
            }

            onExited: {
                selection.visible = false
                favTitle.color = Kirigami.Theme.textColor
                favSubtitle.color = Kirigami.Theme.disabledTextColor
                favTitle.font.bold = false
            }
        }

        width: 200
        height: 200
        radius: 10
        shadow.size: 15
        shadow.xOffset: 5
        shadow.yOffset: 5
        shadow.color: Qt.rgba(0, 0, 0, 0.2)

        Item {
            id: icon
            anchors.fill: parent

            Item {
                id: content
                anchors.fill: parent

                layer.enabled: true
                layer.effect: OpacityMask {
                    maskSource: mask
                }
            }

            Rectangle {
                id: mask
                anchors.fill: content
                visible: false
                radius: 10
            }
        }

        Rectangle {
            id: selection

            Rectangle {
                anchors.fill: parent
                color: Kirigami.Theme.hoverColor
                radius: 10
                opacity: 0.2
            }
            Item {
                visible: root.showIcon

                height: parent.height
                width: parent.width
                NumberAnimation on opacity {
                    id: playAnimationOpacity
                    easing.type: Easing.OutCubic
                    running: false
                    from: 0
                    to: 1
                }
                NumberAnimation on y {
                    id: playAnimationPosition
                    easing.type: Easing.OutCubic
                    running: false
                    from: 20
                    to: 0
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
                    y: 100 - 0.5 * height
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
        Layout.maximumWidth: 210
        ColumnLayout {
            Layout.fillWidth: true
            Controls.Label {
                id: favTitle
                Layout.fillWidth: true
                leftPadding: 5
                elide: Text.ElideRight
            }
            Controls.Label {
                id: favSubtitle
                Layout.fillWidth: true
                leftPadding: 5
                color: Kirigami.Theme.disabledTextColor
                elide: Text.ElideRight
            }
        }
        Controls.ToolButton {
            Layout.fillHeight: true
            icon.name: "overflow-menu"
            onPressed: root.optionsClicked()
        }
    }
}
