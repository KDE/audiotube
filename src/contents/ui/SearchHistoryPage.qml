// SPDX-FileCopyrightText: Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import org.kde.kirigami as Kirigami
import QtQuick.Controls as Controls

import QtQuick
import QtQuick.Layouts

import org.kde.audiotube

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
            visible: searchLoader.text && recentsRepeater.count > 0
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
                    model: LocalSearchModel {
                        searchQuery: searchLoader.text
                    }

                    delegate: searchAlbum
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
            model: Library.searches

            delegate: Controls.ItemDelegate {
                implicitHeight: Kirigami.Units.gridUnit * 2
                Layout.fillWidth: true

                required property string searchQuery

                contentItem: RowLayout {
                    Kirigami.Icon {
                        source: "search"
                        implicitHeight: Kirigami.Units.gridUnit
                        implicitWidth: Kirigami.Units.gridUnit
                        color: Kirigami.Theme.disabledTextColor
                    }
                    Controls.Label {
                        text: searchQuery
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }
                    Controls.ToolButton {
                        icon.name: "list-remove"
                        text: i18n("remove from search history")
                        display: Controls.AbstractButton.IconOnly
                        onClicked: {
                            Library.removeSearch(model.display)
                        }
                    }

                }
                onClicked: {
                    searchLoader.text = searchQuery
                    searchLoader.accepted()
                }
            }
        }
    }
}
