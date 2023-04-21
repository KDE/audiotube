// SPDX-FileCopyrightText: 2022 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm

import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.20 as Kirigami
import org.kde.ytmusic 1.0

MobileForm.AboutPage {
    aboutData: About

    Component.onCompleted: {
        if (hasOwnProperty("showAboutKDE")) {
            showAboutKDE = false
        }
    }
}
