// SPDX-FileCopyrightText: Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import org.kde.kirigami 2.12 as Kirigami
import QtQuick.Controls 2.15 as Controls

import QtQuick 2.15
import QtQuick.Layouts 1.15

import org.kde.ytmusic 1.0

Kirigami.ScrollablePage {
    objectName: "searchPage"

    topPadding: 0
    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    ColumnLayout {
        Controls.ScrollView {
            Layout.fillWidth: true

            id: recents
            visible: searchField.text && recentsRepeater.count > 0
            leftPadding: 10
            topPadding: 10
            Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff

            RowLayout {
                id: recentsLayout
                spacing: 10
                Layout.fillWidth: true

                Repeater {
                    id: recentsRepeater
                    Layout.fillWidth: true
                    model: SortFilterModel {
                        filterRole: PlaybackHistoryModel.Title
                        filterRegularExpression: searchField.filterExpression
                        sourceModel: Library.playbackHistory
                    }
                    delegate: Kirigami.DelegateRecycler {
                        Layout.alignment: Qt.AlignTop
                        sourceComponent: searchAlbum
                    }
                }
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
            visible: recents.visible
        }

        id: completionList

        Repeater {
            Layout.fillHeight: true
            Layout.fillWidth: true
            model: SortFilterModel {
                sourceModel: Library.searches
                filterRegularExpression: searchField.filterExpression
            }

            delegate: Kirigami.AbstractListItem {
                implicitHeight: Kirigami.Units.gridUnit * 2
                Layout.fillWidth: true
                RowLayout {
                    Kirigami.Icon {
                        source: "search"
                        implicitHeight: Kirigami.Units.gridUnit
                        implicitWidth: Kirigami.Units.gridUnit
                        color: Kirigami.Theme.disabledTextColor
                    }
                    Controls.Label {
                        text: model.display
                        Layout.fillWidth: true
                    }

                }
                onClicked: {
                    searchField.text = model.display
                    searchField.accepted()
                }
            }
        }
    }

}
