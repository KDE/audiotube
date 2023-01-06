// SPDX-FileCopyrightText: 2022 Mathis Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.20 as Kirigami
import org.kde.ytmusic 1.0



Rectangle {
    property bool collapsed: false
    id: sidebar
    width: 200
    clip:true
    Kirigami.Theme.colorSet: Kirigami.Theme.Window
    Kirigami.Theme.inherit: false
    color: "transparent"

    Rectangle {
        anchors.fill: parent
        color: Kirigami.Theme.backgroundColor
        opacity: 0.7
    }

    NumberAnimation on width{
        id: collapse
        easing.type: Easing.OutCubic
        running: false
        from: 200; to: 60
    }
    NumberAnimation on width{
        id: show
        easing.type: Easing.OutCubic
        running: false
        from: 60; to: 200
        //this is only a workaround to stop the text from disappearing
        onFinished:{
            libraryButton.checked =!libraryButton.checked
            libraryButton.checked =!libraryButton.checked

            favouritesButton.checked =!favouritesButton.checked
            favouritesButton.checked =!favouritesButton.checked

            historyButton.checked =!historyButton.checked
            historyButton.checked =!historyButton.checked

            searchButton.checked =!searchButton.checked
            searchButton.checked =!searchButton.checked

            collapseButton.checked =!collapseButton.checked
            collapseButton.checked =false

        }
    }
    Kirigami.Separator {
        height: parent.height
        anchors.right: parent.right
        z:100
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Kirigami.Separator {
            Layout.fillWidth: true


        }

        Controls.ScrollView {

            id: scrollView
            Layout.fillWidth: true
            Layout.fillHeight: true

            Controls.ScrollBar.vertical.policy: Controls.ScrollBar.AlwaysOff
            Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff
            contentWidth: -1 // disable horizontal scroll

            ColumnLayout {
                id: column
                width: scrollView.width
                spacing: 0

                Kirigami.NavigationTabButton {
                    id: libraryButton
                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin
                    implicitHeight: 50
                    display: Controls.AbstractButton.TextBesideIcon
                    text: collapsed? "" : i18n("Library")
                    icon.name: "file-library-symbolic"
                    checked: pageStack.currentItem && pageStack.currentItem.objectName == "libraryPage"
                    onClicked: {
                        pageStack.clear()
                        pageStack.push("qrc:/LibraryPage.qml")


                    }
                }
                Kirigami.NavigationTabButton {
                    id: searchButton
                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin
                    implicitHeight: 50
                    display: Controls.AbstractButton.TextBesideIcon
                    text: collapsed? "" : i18n("Search")
                    icon.name: "search"
                    checked: pageStack.currentItem && pageStack.currentItem.objectName == "searchPage"
                    onClicked: {
                        searchField.forceActiveFocus()
                        popup.open()
                    }
                }

                Kirigami.NavigationTabButton {
                    id: favouritesButton
                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin
                    implicitHeight: 50
                    display: Controls.AbstractButton.TextBesideIcon
                    text: collapsed ? "" : i18n("Favourites")
                    icon.name: "non-starred-symbolic"
                    checked: pageStack.currentItem && pageStack.currentItem.objectName == "favourites"
                    onClicked: {
                        pageStack.clear()
                        pageStack.push("qrc:/PlaybackHistory.qml", {
                          "dataModel": Library.favourites,
                          "title": i18n("Favourites"),
                          "objectName": "favourites"
                      })}

                }

                Kirigami.NavigationTabButton {
                    id: historyButton
                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin
                    implicitHeight: 50
                    display: Controls.AbstractButton.TextBesideIcon
                    text: collapsed? "" : i18n("Played Songs")
                    icon.name: "edit-undo-history"
                    checked: pageStack.currentItem && pageStack.currentItem.objectName == "history"
                    onClicked: {
                        pageStack.clear()
                        pageStack.push("qrc:/PlaybackHistory.qml", {
                          "dataModel": Library.playbackHistory,
                          "title": i18n("Played Songs"),
                          "objectName": "history"
                      })}

                }

//                Kirigami.NavigationTabButton {
//                    Layout.fillWidth: true
//                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin
//                    implicitHeight: 50
//                    display: Controls.AbstractButton.TextBesideIcon
//                    text: i18n("Charts")
//                    icon.name: "office-chart-line-forecast"

//                }
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
            Layout.rightMargin: Kirigami.Units.smallSpacing
            Layout.leftMargin: Kirigami.Units.smallSpacing
        }

        Kirigami.NavigationTabButton {
            id: collapseButton
            Layout.fillWidth: true
            width: column.width - column.Layout.leftMargin - column.Layout.rightMargin
            display: Controls.AbstractButton.TextBesideIcon
            text: collapsed? "" : i18n("Collapse Sidebar")
            checked: collapsed == true
            implicitHeight: 50
            icon.name: "sidebar-collapse"
            onClicked: {
                if (!sidebar.collapsed)
                    collapse.running = true
                else
                    show.running = true

                sidebar.collapsed = !sidebar.collapsed
            }

        }
    }
}
