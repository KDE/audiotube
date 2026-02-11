// SPDX-FileCopyrightText: 2026 Tanveer Ahmed Mansuri
// SPDX-FileCopyrightText: 2026 Carl Schwan <carlschwan@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.audiotube
import QtQml

ColumnLayout {
    id: root

    required property string title
    required property var contentModel
    required property SongMenu songMenu

    spacing: Kirigami.Units.largeSpacing

    Kirigami.Heading {
        text: root.title
        Layout.fillWidth: true
    }

    Flickable {
        Layout.fillWidth: true
        Layout.preferredHeight: 180
        Layout.topMargin: Kirigami.Units.largeSpacing
        
        contentWidth: shelfRow.implicitWidth + Kirigami.Units.gridUnit
        contentHeight: shelfRow.implicitHeight
        flickableDirection: Flickable.HorizontalFlick
        
        RowLayout {
            id: shelfRow
            x: 0
            spacing: Kirigami.Units.gridUnit
            
            Repeater {
                model: root.contentModel
                delegate: BentoCard {
                    // Fixed 1x1 Card
                    wide: false
                    tall: false
                    
                    Layout.preferredWidth: 180
                    Layout.preferredHeight: 180

                    Layout.fillWidth: true 
                    Layout.fillHeight: true
                    
                    onClicked: root.contentModel.triggerItem(index)
                    Controls.ContextMenu.onRequested: if (isSong && videoId && title && artists) {
                        root.songMenu.openForSong(videoId, title, artists, artistsDisplayString)
                    }
                }
            }
        }
    }
}
