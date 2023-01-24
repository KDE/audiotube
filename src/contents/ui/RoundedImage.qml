// SPDX-FileCopyrightText: 2022 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import QtQuick.Window 2.15
import Qt5Compat.GraphicalEffects

Item {
    property alias radius: mask.radius
    property alias source: image.source
    id: icon

    Image {
        id:image
        source: source
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        asynchronous: true
        sourceSize.width: parent.implicitWidth * Screen.devicePixelRatio
        layer.enabled: true
        layer.effect: OpacityMask {
            maskSource: mask
        }
    }

    Rectangle {
        id: mask
        anchors.fill: parent
        visible: false
    }
}

