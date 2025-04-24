// SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami as Kirigami

Kirigami.InlineMessage {

    property var okAction
    property var cancelAction

    visible: false
    actions: [
        Kirigami.Action {
            text: i18n("OK")

            onTriggered: okAction()
        },
        Kirigami.Action {
            text: i18n("Cancel")

            onTriggered: cancelAction()
        }
    ]
}
