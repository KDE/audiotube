// SPDX-FileCopyrightText: 2026 Tanveer Ahmed Mansuri
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.audiotube
import QtQml

Kirigami.ScrollablePage {
    id: root
    title: moodTitle
    objectName: "moodPage"
    
    required property string moodTitle
    required property string moodParams
    
    MoodPlaylistsModel {
        id: moodModel
        params: root.moodParams
        title: root.moodTitle
        onOpenPlaylist: (browseId) => applicationWindow().playPlaylist(browseId)
    }
    
    GridView {
        id: grid

        model: moodModel

        leftMargin: Kirigami.Units.gridUnit
        rightMargin: Kirigami.Units.gridUnit
        topMargin: Kirigami.Units.gridUnit
        bottomMargin: Kirigami.Units.gridUnit

        readonly property int viewWidth: grid.width - Kirigami.Units.gridUnit * 2
        readonly property int columns: Math.max(Math.floor(viewWidth / (180 + Kirigami.Units.gridUnit)), 2)

        cellWidth: Math.floor(viewWidth / columns);
        cellHeight: 180 + Kirigami.Units.gridUnit

        reuseItems: true
        activeFocusOnTab: true
        keyNavigationEnabled: true
        
        delegate: BentoCard {
            width: 180
            height: 180
            
            required property int itemCount
            required property string author

            artistsDisplayString: itemCount ? i18ncp("song count", "%1 song", "%1 songs", itemCount) : (author || "")
            onClicked: moodModel.triggerItem(index)
        }
    }
}
