// SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami

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
