// SPDX-FileCopyrightText: 2023 Jonah Brüchert <jbb@kaidan.im>
// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

Controls.ToolButton {
    id: root
    property alias albumCover: content.children
    property alias title: favTitle.text
    property alias subtitle: favSubtitle.text
    property bool showIcon: true

    signal clicked
    signal optionsClicked

    implicitWidth: mainLayout.implicitWidth
    implicitHeight: mainLayout.implicitHeight

    background: Item {}

    MouseArea {
        id: coverArea
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        anchors.fill: parent
        onClicked: (mouse) => {
           if (mouse.button === Qt.RightButton) {
                root.optionsClicked()
            } else if (mouse.button === Qt.LeftButton) {
                root.clicked()
            }
        }

        hoverEnabled: !Kirigami.Settings.hasTransientTouchInput
        onEntered: {
            if (!Kirigami.Settings.hasTransientTouchInput) {
                playAnimationPosition.running = true
                playAnimationOpacity.running = true
            }
        }
    }

    ColumnLayout {
        id: mainLayout

        anchors.fill: parent
        Kirigami.ShadowedRectangle {
            id: cover

            color: Kirigami.Theme.backgroundColor
            Layout.margins: 5
            width: 200
            height: 200
            radius: 10
            shadow.size: 15
            shadow.xOffset: 5
            shadow.yOffset: 5
            shadow.color: Qt.rgba(0, 0, 0, 0.2)

            Item {
                id: content
                anchors.fill: parent
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
                visible: coverArea.containsMouse || root.activeFocus
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
                    color: coverArea.containsMouse || root.activeFocus
                                               ? Kirigami.Theme.hoverColor
                                               : Kirigami.Theme.textColor
                    font.bold: coverArea.containsMouse || root.activeFocus
                }
                Controls.Label {
                    id: favSubtitle
                    Layout.fillWidth: true
                    leftPadding: 5
                    color: coverArea.containsMouse || root.activeFocus
                           ? Kirigami.Theme.hoverColor
                           : Kirigami.Theme.disabledTextColor
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
}

