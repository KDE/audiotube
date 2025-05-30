// SPDX-FileCopyrightText: 2022 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.audiotube

Rectangle {
    property int sidebarWidth: 230
    property int sidebarWidthCollapsed: 60
    property bool collapsed: true

    // workaround for label not being displayed before collapsed change
    Component.onCompleted: {
        collapsed = false
    }

    id: sidebar
    width: sidebarWidth
    clip:true
    Kirigami.Theme.colorSet: Kirigami.Theme.Window
    Kirigami.Theme.inherit: false
    color: "transparent"

    Rectangle {
        anchors.fill: parent
        color: Kirigami.Theme.backgroundColor
        opacity: 0.8
    }

    NumberAnimation on width{
        id: collapse
        easing.type: Easing.OutCubic
        running: false
        from: sidebarWidth; to: sidebarWidthCollapsed
    }
    NumberAnimation on width{
        id: show
        easing.type: Easing.OutCubic
        running: false
        from: sidebarWidthCollapsed; to: sidebarWidth
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

            aboutButton.checked = !aboutButton.checked
            aboutButton.checked = !aboutButton.checked

            playlistsButton.checked = !playlistsButton.checked
            playlistsButton.checked = !playlistsButton.checked
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
                    display: collapsed ? Controls.AbstractButton.IconOnly : Controls.AbstractButton.TextBesideIcon
                    checked: pageStack.currentItem && pageStack.currentItem.objectName == "libraryPage"

                    action: Kirigami.PagePoolAction {
                        pagePool: pool
                        text: i18n("Library")
                        icon.name: "file-library-symbolic"
                        page: Qt.resolvedUrl("qrc:/qt/qml/org/kde/audiotube/contents/ui/LibraryPage.qml")
                    }
                }
                Kirigami.NavigationTabButton {
                    id: searchButton
                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin
                    implicitHeight: 50
                    display: collapsed ? Controls.AbstractButton.IconOnly : Controls.AbstractButton.TextBesideIcon
                    text: i18n("Search")
                    icon.name: "search"
                    checked: pageStack.currentItem && pageStack.currentItem.objectName == "searchPage"
                    onClicked: {
                        searchField.forceActiveFocus()
                        focusSearch()
                    }
                }

                Kirigami.NavigationTabButton {
                    id: favouritesButton
                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin
                    implicitHeight: 50
                    display: collapsed ? Controls.AbstractButton.IconOnly : Controls.AbstractButton.TextBesideIcon
                    checked: pageStack.currentItem && pageStack.currentItem.objectName == "favourites"

                    action: Kirigami.PagePoolAction {
                        pagePool: pool
                        text: i18n("Favourites")
                        icon.name: "non-starred-symbolic"
                        page: Qt.resolvedUrl("qrc:/qt/qml/org/kde/audiotube/contents/ui/PlaybackHistory.qml#favourites")
                        initialProperties: {
                            "dataModel": Library.favourites,
                            "title": i18n("Favourites"),
                            "objectName": "favourites"
                        }
                    }
                }

                Kirigami.NavigationTabButton {
                    id: historyButton
                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin
                    implicitHeight: 50
                    display: collapsed ? Controls.AbstractButton.IconOnly : Controls.AbstractButton.TextBesideIcon
                    checked: pageStack.currentItem && pageStack.currentItem.objectName == "history"

                    action: Kirigami.PagePoolAction {
                        pagePool: pool
                        text: i18n("Played Songs")
                        icon.name: "edit-undo-history"
                        page: Qt.resolvedUrl("qrc:/qt/qml/org/kde/audiotube/contents/ui/PlaybackHistory.qml#history")
                        initialProperties: {
                            "dataModel": Library.playbackHistory,
                            "title": i18n("Played Songs"),
                            "objectName": "history"
                        }
                    }
                }
                Kirigami.NavigationTabButton {
                    id: playlistsButton
                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin
                    implicitHeight: 50
                    display: collapsed ? Controls.AbstractButton.IconOnly : Controls.AbstractButton.TextBesideIcon
                    checked: pageStack.currentItem && pageStack.currentItem.objectName == "playlists"

                    action: Kirigami.PagePoolAction {
                        pagePool: pool
                        text: i18n("Playlists")
                        icon.name: "amarok_playlist"
                        page: Qt.resolvedUrl("qrc:/qt/qml/org/kde/audiotube/contents/ui/LocalPlaylistsPage.qml")
                        initialProperties: {
                            "objectName": "playlists"
                        }
                    }
                }
            }
        }
        Kirigami.Separator {
            Layout.fillWidth: true
            Layout.rightMargin: Kirigami.Units.smallSpacing
            Layout.leftMargin: Kirigami.Units.smallSpacing
        }
        Kirigami.NavigationTabButton {
            id: aboutButton
            Layout.fillWidth: true
            width: column.width - column.Layout.leftMargin - column.Layout.rightMargin
            implicitHeight: 50
            display: collapsed ? Controls.AbstractButton.IconOnly : Controls.AbstractButton.TextBesideIcon
            text: i18n("About")
            checkable: false
            icon.name: "help-about-symbolic"

            onClicked: {
                pageStack.pushDialogLayer(Qt.createComponent("org.kde.kirigamiaddons.formcard", "AboutPage"))
            }
        }

        Kirigami.NavigationTabButton {
            id: collapseButton
            Layout.fillWidth: true
            width: column.width - column.Layout.leftMargin - column.Layout.rightMargin
            display: collapsed ? Controls.AbstractButton.IconOnly : Controls.AbstractButton.TextBesideIcon
            text: i18n("Collapse Sidebar")
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
