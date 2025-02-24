// SPDX-FileCopyrightText: 2022 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import QtQuick.Effects

import org.kde.kirigami 2.14 as Kirigami
import org.kde.kirigami.delegates as KirigamiDelegates
import org.kde.kirigamiaddons.components 1.0 as Components

import QtQuick.Controls 2.14 as Controls
import QtQuick.Layouts 1.3

import org.kde.ytmusic 1.0


Item {
    id: root

    required property string imageSourceURL
    property bool rounded: false
    required property string title
    property string subtitle
    property list<Kirigami.Action> visibleActions
    property list<Kirigami.Action> overflowActions

    height: root.width > 500 ? 200 : 400

    Item {
        anchors.fill: parent

        Image {
            scale: 1.8
            anchors.fill: parent
            asynchronous: true

            source: imageSourceURL
            fillMode: Image.PreserveAspectCrop

            sourceSize.width: 512
            sourceSize.height: 512
        }

        layer.enabled: true
        layer.effect: MultiEffect {
            brightness: -0.25
            saturation: 0.5

            blurEnabled: true
            autoPaddingEnabled: false
            blur: 1.0
            blurMax: 40
            blurMultiplier: 3.0
        }
    }
    Rectangle {
        anchors.fill: parent
        gradient: Gradient{
            GradientStop { position: -1.0; color: "transparent" }
            GradientStop { position: 1.0; color: Kirigami.Theme.backgroundColor }

        }
    }
    GridLayout {
        width: parent.width

        flow: root.width > 500?GridLayout.LeftToRight:GridLayout.TopToBottom
        Kirigami.ShadowedRectangle {
            Layout.alignment: Qt.AlignHCenter
            color: Kirigami.Theme.backgroundColor
            Layout.margins: 30
            width: 150
            height: width
            radius: rounded ? 100 : 10
            shadow.size: 15
            shadow.xOffset: 5
            shadow.yOffset: 5
            shadow.color: Qt.rgba(0, 0, 0, 0.2)

            RoundedImage {
                source: imageSourceURL
                height: parent.height
                width: height
                radius: rounded ? 100 : 10
            }
        }
        ColumnLayout {
            Layout.leftMargin: 30
            Layout.rightMargin: 30
            Layout.fillWidth: true

            Controls.Label {
                horizontalAlignment:  (root.width <= 500)? Qt.AlignHCenter: Qt.AlignLeft
                Layout.fillWidth: true
                text: title
                font.bold: true
                font.pixelSize: 22
                elide: Qt.ElideRight
            }
            Controls.Label {
                horizontalAlignment:  (root.width <= 500)? Qt.AlignHCenter: Qt.AlignLeft
                Layout.fillWidth: true
                text: subtitle
                elide: Qt.ElideRight
            }
            RowLayout {
                visible: root.visibleActions.length>0
                Layout.topMargin: 30

                Repeater {
                    model: root.visibleActions
                    delegate: Controls.RoundButton{
                        required property var modelData
                        Layout.fillWidth: root.width <= 500
                        leftPadding: 20
                        rightPadding: 20
                        icon.name: modelData.icon.name
                        text:modelData.text
                        onClicked: modelData.triggered()
                        visible: modelData.visible
                    }
                }
                Controls.Menu {
                    id: overflowMenu
                    Instantiator {
                        model: root.overflowActions
                        onObjectAdded: (index, object) => overflowMenu.insertItem(index, object)
                        onObjectRemoved: (index, object) => overflowMenu.removeItem(object)
                        delegate: Controls.MenuItem {
                            required property var modelData
                            icon.name: modelData.icon.name
                            text: modelData.text
                            visible: modelData.visible
                            onTriggered: modelData.triggered()
                        }
                    }
                }

                Components.BottomDrawer {
                    id: overflowDrawer

                    parent: applicationWindow().overlay

                    drawerContentItem: ColumnLayout {
                        Repeater {
                            model: root.overflowActions

                            delegate: Controls.ItemDelegate {
                                required property var modelData

                                Layout.fillWidth: true
                                text: modelData.text
                                icon.name: modelData.icon.name
                                visible: modelData.visible
                                onClicked: {
                                    modelData.triggered()
                                    overflowDrawer.close()
                                    overflowDrawer.interactive = false
                                }
                            }
                        }
                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }
                Controls.RoundButton {
                    onClicked: {
                        if (Kirigami.Settings.isMobile) {
                            overflowDrawer.open()
                            overflowDrawer.interactive = true
                        } else {
                            overflowMenu.popup()
                        }
                    }
                    visible: root.overflowActions.length > 0
                    icon.name: "overflow-menu"
                }
            }
        }
    }
}
