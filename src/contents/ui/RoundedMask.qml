// SPDX-FileCopyrightText: Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15

Rectangle {
    id: mask

    radius: 10
    layer.enabled: true
    layer.samplerName: "maskSource"
    property var colorSource

    layer.effect: ShaderEffect {
        property var colorSource: mask.colorSource
        fragmentShader: "
            uniform lowp sampler2D colorSource;
            uniform lowp sampler2D maskSource;
            uniform lowp float qt_Opacity;
            varying highp vec2 qt_TexCoord0;
            void main() {
                gl_FragColor = texture2D(colorSource, qt_TexCoord0) * texture2D(maskSource, qt_TexCoord0).a * qt_Opacity;
            }"
    }
}
