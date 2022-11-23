// SPDX-FileCopyrightText: 2022 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15

Rectangle {
    property alias radius: mask.radius
    property alias source: image.source
    id: icon
    color: "transparent"

    Rectangle {
        id: imageSource
        anchors.fill: parent
        Image {
            id:image
            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop
            asynchronous: true
        }
        visible: false

        layer.enabled: true
    }

    RoundedMask {
        id: mask
        anchors.fill: parent
        colorSource: imageSource
    }
}
