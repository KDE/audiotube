// SPDX-FileCopyrightText: 2022 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Window

import org.kde.kirigami as Kirigami

Item {
    id: icon

    property int radius
    property alias source: image.source

    Image {
        id: image

        source: source
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        asynchronous: true
        sourceSize.width: parent.implicitWidth * Screen.devicePixelRatio

        layer {
            enabled: GraphicsInfo.api !== GraphicsInfo.Software
            effect: Kirigami.ShadowedTexture {
                id: mask
                radius: icon.radius
            }
        }
    }
}

