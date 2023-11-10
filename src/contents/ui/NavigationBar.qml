// SPDX-FileCopyrightText: 2022 Mathis Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.20 as Kirigami

import org.kde.ytmusic 1.0

Kirigami.NavigationTabBar {
    id: root
    anchors.bottom: parent.bottom
    width:parent.width
    backgroundColor: "transparent"
    foregroundColor: "white"
    highlightForegroundColor: "white"

    actions: [
        Kirigami.Action {
            icon.name: "file-library-symbolic"
            text: i18n("Library")
            checked: pageStack.currentItem && pageStack.currentItem.objectName == "libraryPage"
            onTriggered: {
                pageStack.clear()
                pageStack.replace("qrc:/LibraryPage.qml")
                applicationWindow().showSearch = false


            }
        },
        Kirigami.Action {
            icon.name: "search"
            text: i18n("Search")
            checked: pageStack.currentItem && pageStack.currentItem.objectName == "searchPage"
            onTriggered: {
                pageStack.clear()
                pageStack.replace("qrc:/SearchHistoryPage.qml",)
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
                pageStack.replace("qrc:/PlaybackHistory.qml", {
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
                pageStack.replace("qrc:/PlaybackHistory.qml", {
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
                pageStack.replace("qrc:/LocalPlaylistsPage.qml", {
                  "objectName": "playlists"
                })
                applicationWindow().showSearch = false

            }
        }
    ]
}
