//SPDX-FileCopyrightText: 2015 Jolla Ltd. <valerio.valerio@jolla.com>
//SPDX-FileContributor: Andres Gomez
//
//SPDX-License-Identifier: LGPL-2.1-or-later


import QtQuick 2.0
import org.nemomobile.qtmpris 1.0

Item {
    id: mainItem

    anchors.fill: parent

    Loader {
        id: controlsLoader

        active: mprisManager.availableServices.length > 0

        Component.onCompleted: setSource("MprisControls.qml", { "mprisManager": mprisManager, "parent": mainItem })

        MprisManager {
            id: mprisManager
        }
    }
}
