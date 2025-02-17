// SPDX-FileCopyrightText: 2022 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15 as Controls

import org.kde.kirigami as Kirigami

Item {
    property int radius
    property alias source1: image1.source
    property alias source2: image2.source
    property alias source3: image3.source
    property alias source4: image4.source
    property string title

    id: icon

    layer {
        enabled: GraphicsInfo.api !== GraphicsInfo.Software
        effect: Kirigami.ShadowedTexture {
            id: mask
            radius: icon.radius
        }
    }

    Rectangle{
        anchors.fill: parent
        color: Qt.rgba(Math.random(),Math.random(),Math.random(),0.4);
    }
    Image {
        id:image1
        anchors.top: parent.top
        anchors.left: parent.left
        width: parent.width/2
        height: parent.height/2
        fillMode: Image.PreserveAspectCrop
        asynchronous: true
        sourceSize.width: parent.implicitWidth * Screen.devicePixelRatio
        Controls.Label{
            z:-1
            text: icon.title.charAt(0)
            anchors.centerIn: parent
            color: "White"
            font.pixelSize: 40
            font.capitalization: Font.AllUppercase
            font.family: "Noto Serif"
            font.bold: true
            enabled: false

        }
    }
    Image {
        id:image2
        anchors.top: parent.top
        anchors.right: parent.right
        width: parent.width/2
        height: parent.height/2
        fillMode: Image.PreserveAspectCrop
        asynchronous: true
        sourceSize.width: parent.implicitWidth * Screen.devicePixelRatio
        Controls.Label{
            z:-1
            text: icon.title.charAt(1)
            anchors.centerIn: parent
            color: "White"
            font.pixelSize: 40
            font.capitalization: Font.AllUppercase
            font.family: "Noto Serif"
            font.bold: true
            enabled: false

        }

    }
    Image {
        id:image3
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: parent.width/2
        height: parent.height/2
        fillMode: Image.PreserveAspectCrop
        asynchronous: true
        sourceSize.width: parent.implicitWidth * Screen.devicePixelRatio
        Controls.Label{
            z:-1
            text: icon.title.charAt(2)
            anchors.centerIn: parent
            color: "White"
            font.pixelSize: 40
            font.capitalization: Font.AllUppercase
            font.family: "Noto Serif"
            font.bold: true
            enabled: false

        }

    }
    Image {
        id:image4
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: parent.width/2
        height: parent.height/2
        fillMode: Image.PreserveAspectCrop
        asynchronous: true
        sourceSize.width: parent.implicitWidth * Screen.devicePixelRatio
        Controls.Label{
            z:-1
            text: icon.title.charAt(3)
            anchors.centerIn: parent
            color: "White"
            font.pixelSize: 40
            font.capitalization: Font.AllUppercase
            font.family: "Noto Serif"
            font.bold: true
            enabled: false

        }
    }



    Rectangle {
        id: mask
        anchors.fill: parent
        visible: false
    }
}

