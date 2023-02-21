// SPDX-FileCopyrightText: 2022 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.14 as Controls
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import org.kde.ytmusic 1.0


Item {
    id:root

    required property string title
    property string subtitle
    property list<Kirigami.Action> visibleActions
    property list<Kirigami.Action> overflowActions
    property int spacing: 20
    property QtObject model
    width:parent.width
    clip:true
    height: root.width > 500 ? 200 : 250

    Item{
        anchors.fill:parent
        layer.enabled: true
        layer.effect: HueSaturation {
            cached: true


            layer.enabled: true
            layer.effect: FastBlur {
                cached: true
                radius: 80
            }
        }
        GridLayout {

            x: -250
            y: -400
            rowSpacing: columnSpacing
            columnSpacing: 30
            rotation: -45
            height: 200
            columns: 10
            rows: 10
            z:-1
            Repeater {
                model: root.model

                delegate: RoundedImage{
                    ThumbnailSource {
                        id: thumbnailSourceBlur
                        videoId: model.videoId
                    }
                    width: 110
                    height: 110
                    radius: 10
                    source: thumbnailSourceBlur.cachedPath}


            }
        }
    }

    Item{
        anchors.fill:parent

        GridLayout {

            x: -250
            y: -400
            rowSpacing: columnSpacing
            columnSpacing: 30
            rotation: -45
            height: 200
            columns: 10
            rows: 10
            z:-1
            Repeater {
                model: root.model

                delegate: RoundedImage{
                    ThumbnailSource {
                        id: thumbnailSource
                        videoId: model.videoId
                    }
                    width: 110
                    height: 110
                    radius: 10
                    source: thumbnailSource.cachedPath}


            }
        }
    }

    Rectangle {
        anchors.fill: parent
        height:200
        width: root.width
        gradient: Gradient{
            GradientStop { position: -1.0; color: "transparent" }
            GradientStop { position: 1.0; color: Kirigami.Theme.backgroundColor }

        }
    }


    ColumnLayout {
        width: parent.width
        Controls.Label {
            Layout.topMargin: (root.width <= 500)?90:40
            Layout.leftMargin: 30
            Layout.fillWidth: true
            text: title
            font.bold: true
            font.pixelSize: 22
            elide: Qt.ElideRight

        }
        Controls.Label {
            Layout.leftMargin: 30
            Layout.fillWidth: true
            text: subtitle
            elide: Qt.ElideRight

        }
        RowLayout {
            Layout.leftMargin: 30
            visible: root.visibleActions.length>0
            Layout.topMargin: 30
            Layout.rightMargin: 30

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
                }
            }
            Controls.Menu {
                id: overflowMenu
                Instantiator {
                    model: root.overflowActions
                    onObjectAdded: overflowMenu.insertItem(index, object)
                    onObjectRemoved: overflowMenu.removeItem(object)
                    delegate: Controls.MenuItem {
                        required property var modelData
                        icon.name: modelData.icon.name
                        text:modelData.text
                        onTriggered: modelData.triggered()
                    }
                }
            }
            Controls.RoundButton {
                onClicked: overflowMenu.popup()
                visible: root.overflowActions.length > 0
                icon.name: "overflow-menu"
            }
        }
    }
}

