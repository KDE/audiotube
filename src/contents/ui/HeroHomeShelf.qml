// SPDX-FileCopyrightText: 2026 Tanveer Ahmed Mansuri
// SPDX-FileCopyrightText: 2026 Carl Schwan <carlschwan@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQml
import QtQml.Models
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.audiotube

ColumnLayout {
    id: root

    required property string title
    required property var contentModel
    required property SongMenu songMenu
    required property Kirigami.Page page

    spacing: 0
    Layout.fillWidth: true

    Kirigami.Heading {
        text: root.title
        Layout.fillWidth: true
    }
    
    Repeater {
        model: root.contentModel
        delegate: DelegateChooser {
            DelegateChoice {
                row: 0
                HeroCard {
                    Layout.topMargin: Kirigami.Units.largeSpacing
                    onPlayClicked: root.contentModel.triggerItem(index)
                    onOpenClicked: root.contentModel.triggerItem(index)
                }
            }
        }
    }

    Flickable {
        Layout.fillWidth: true
        Layout.preferredHeight: bentoGrid.implicitHeight
        Layout.topMargin: Kirigami.Units.gridUnit

        contentWidth: bentoGrid.implicitWidth + Kirigami.Units.gridUnit
        contentHeight: bentoGrid.implicitHeight
        flickableDirection: Flickable.HorizontalFlick
        
        GridLayout {
            id: bentoGrid
            x: 0
            
            rows: 2
            flow: GridLayout.TopToBottom
            rowSpacing: Kirigami.Units.gridUnit
            columnSpacing: Kirigami.Units.gridUnit
        
            Repeater {
                model: root.contentModel
                delegate: DelegateChooser {
                    DelegateChoice {
                        row: 0
                        Item { visible: false }
                    }

                    DelegateChoice {
                        BentoCard {
                            // Ensure inactive items don't take space
                            property int baseWidth: 180
                            
                            Layout.preferredWidth: wide ? (baseWidth * 2 + spacing) : baseWidth
                            Layout.preferredHeight: baseWidth
                            Layout.columnSpan: wide ? 2 : 1

                            Layout.fillWidth: true 
                            Layout.fillHeight: true
                    
                            wide: (index % 6 === 1) || (index % 6 === 5)
                            spacing: Kirigami.Units.gridUnit
                        
                            onClicked: root.contentModel.triggerItem(index)
                            Controls.ContextMenu.onRequested: if (isSong && videoId && title && artists) {
                                root.songMenu.openForSong(videoId, title, artists, artistsDisplayString)
                            }
                        }
                    }
                }
            }
        }
    }
}
