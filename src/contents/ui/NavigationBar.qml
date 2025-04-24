// SPDX-FileCopyrightText: 2022 Mathis Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

import org.kde.audiotube

Kirigami.NavigationTabBar {
    id: root
    anchors.bottom: parent.bottom
    width:parent.width
    Kirigami.Theme.backgroundColor: "transparent"
    Kirigami.Theme.textColor: "white"
    Kirigami.Theme.highlightedTextColor: "white"

    actions: [
        Kirigami.Action {
            icon.name: "file-library-symbolic"
            text: i18n("Library")
            checked: pageStack.currentItem && pageStack.currentItem.objectName == "libraryPage"
            onTriggered: {
                pageStack.clear()
                pageStack.replace("qrc:/qt/qml/org/kde/audiotube/contents/ui/LibraryPage.qml")
                applicationWindow().showSearch = false


            }
        },
        Kirigami.Action {
            icon.name: "search"
            text: i18n("Search")
            checked: pageStack.currentItem && pageStack.currentItem.objectName == "searchPage"
            onTriggered: {
                pageStack.clear()
                pageStack.replace("qrc:/qt/qml/org/kde/audiotube/contents/ui/SearchHistoryPage.qml",)
                applicationWindow().showSearch = true
                applicationWindow().searchField.forceActiveFocus();
            }

        },
        Kirigami.Action {
            icon.name: "non-starred-symbolic"
            text: i18n("Favourites")
            checked: pageStack.currentItem && pageStack.currentItem.objectName == "favourites"
            onTriggered: {
                pageStack.clear()
                pageStack.replace("qrc:/qt/qml/org/kde/audiotube/contents/ui/PlaybackHistory.qml", {
                                                "dataModel": Library.favourites,
                                                "title": i18n("Favourites"),
                                                "objectName": "favourites"})
                applicationWindow().showSearch = false
            }
        },
        Kirigami.Action {
            icon.name: "edit-undo-history"
            text: i18n("Played Songs")
            checked: pageStack.currentItem && pageStack.currentItem.objectName == "history"
            onTriggered: {
                pageStack.clear()
                pageStack.replace("qrc:/qt/qml/org/kde/audiotube/contents/ui/PlaybackHistory.qml", {
                                                "dataModel": Library.playbackHistory,
                                                "title": i18n("Played Songs"),
                                                "objectName": "history"})
                applicationWindow().showSearch = false

            }
        },
        Kirigami.Action {
            icon.name: "amarok_playlist"
            text: i18n("Playlists")
            checked: pageStack.currentItem && pageStack.currentItem.objectName == "playlists"
            onTriggered: {
                pageStack.clear()
                pageStack.replace("qrc:/qt/qml/org/kde/audiotube/contents/ui/LocalPlaylistsPage.qml", {
                  "objectName": "playlists"
                })
                applicationWindow().showSearch = false

            }
        }
    ]
}
