// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as Controls

Controls.ScrollView {
    property alias model: listView.model
    property alias delegate: listView.delegate
    property alias count: listView.count
    property alias itemSpacing: listView.spacing

    height: 250
    contentHeight: 265

    ListView {
        id: listView
        orientation: Qt.Horizontal
        spacing: 10
        height: 250

        reuseItems: true

        header: Item {
            width: 20
        }
    }
}
