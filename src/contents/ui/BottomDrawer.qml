// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import org.kde.ytmusic 1.0
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Layouts 1.15


Controls.Drawer {
    id: root
    property alias drawerContentItem: control.contentItem

    edge: Qt.BottomEdge
    height:contents.implicitHeight+20
    width: applicationWindow().width
    interactive: false
    background: Kirigami.ShadowedRectangle{
        corners.topRightRadius: 10
        corners.topLeftRadius: 10
        shadow.size: 20
        shadow.color: Qt.rgba(0, 0, 0, 0.5)
        color: Kirigami.Theme.backgroundColor

    }
    onAboutToShow: root.interactive = true
    onClosed: root.interactive = false
    ColumnLayout {
        id: contents
        anchors.fill: parent
        Rectangle {
            Layout.margins: 5
            radius:50
            Layout.alignment: Qt.AlignHCenter
            color: Kirigami.Theme.textColor
            opacity: 0.5
            width: 40
            height: 4

        }
        Controls.Control {
            topPadding: 0
            leftPadding: 0
            rightPadding: 0
            bottomPadding: 0
            Layout.margins: 0
            Layout.fillHeight: true
            Layout.fillWidth: true
            id: control
        }
    }
}
