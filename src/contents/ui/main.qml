// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.1
import QtQuick.Controls 2.12 as Controls
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.14 as Kirigami

import org.kde.ytmusic 1.0

Kirigami.ApplicationWindow {
    id: root

    property alias searchField: searchLoader.item // TODO

    property bool wideScreen: width >= 600
    property bool showSearch: false // only applicable if not widescreen

    header: Controls.Control {
        padding: Kirigami.Units.largeSpacing

        background: Rectangle {
             width: parent.width
             height: parent.height
             anchors.fill: parent
             Kirigami.Theme.inherit: false
             Kirigami.Theme.colorSet: Kirigami.Theme.Header
             color:  Kirigami.Theme.backgroundColor
         }

        contentItem: RowLayout {
            Controls.ToolButton {
                id: searchBack
                Layout.alignment: Qt.AlignLeft
                visible:
                    (!root.wideScreen && root.showSearch && root.searchField.text === "")
                    || pageStack.layers.depth > 1
                text: i18n("Back")
                icon.name: "go-previous-view"
                display: Controls.ToolButton.IconOnly
                onClicked: if (root.showSearch)
                               root.showSearch = false
                           else
                               pageStack.layers.pop()
            }

            Kirigami.Heading {
                Layout.alignment: Qt.AlignLeft
                Layout.leftMargin: Kirigami.Units.largeSpacing
                level: 1
                text: "AudioTube"
                visible: !root.wideScreen && !root.showSearch
            }

            // spacer
            Item {
                Layout.fillWidth: !root.wideScreen
            }

            Loader {
                id: searchLoader
                visible: root.wideScreen || root.showSearch
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Layout.maximumWidth: 400
                sourceComponent: searchFieldComponent
            }

            Controls.ToolButton {
                Layout.alignment: Qt.AlignRight
                visible: !root.wideScreen && !root.showSearch
                text: i18n("Search")
                icon.name: "search"
                display: Controls.ToolButton.TextBesideIcon
                onClicked: {
                    root.showSearch = true
                    root.searchField.forceActiveFocus();
                    root.searchField.popup.open();
                }
            }
        }
    }

    title: i18n("AudioTube")

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }

    pageStack.initialPage: "qrc:/LibraryPage.qml"

    function play(videoId) {
        UserPlaylistModel.initialVideoId = videoId
    }

    function playPlaylist(playlistId) {
        UserPlaylistModel.playlistId = playlistId
    }

    function playShufflePlaylist(playlistId) {
        UserPlaylistModel.shuffle = true
        UserPlaylistModel.playlistId = playlistId
    }


    Connections {
        target: ErrorHandler

        function onErrorOccurred(error) {
            showPassiveNotification(error)
        }
    }
    
    Component {
        id: searchFieldComponent
        
        Kirigami.SearchField {
            id: searchField
            delaySearch: true
            selectByMouse: true
            onPressed: {
                popup.open()
            }
            
            property alias popup: popup

            Controls.Popup {
                id: popup
                x: searchField.y
                y: searchField.y + searchField.height
                width: searchField.width
                height: completionList
                        ? Math.min(completionList.count * Kirigami.Units.gridUnit * 2 + Kirigami.Units.gridUnit * 2, Kirigami.Units.gridUnit * 20)
                        : Kirigami.Units.gridUnit * 20

                contentItem: Controls.ScrollView {
                    Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff
                    ListView {
                        id: completionList
                        model: Library.searches
                        delegate: Controls.ItemDelegate {
                            Kirigami.Theme.colorSet: Kirigami.Theme.Window
                            Kirigami.Theme.inherit: false
                            id: completionDelegate
                            width: parent.width
                            height: Kirigami.Units.gridUnit * 2
                            text: model.display
                            onClicked: {
                                searchField.text = model.display
                                searchField.accepted()
                            }
                        }
                    }
                }
            }

            onAccepted: {
                pageStack.clear()
                pageStack.layers.clear()

                if (text) {
                    Library.addSearch(text)
                    pageStack.push("qrc:/SearchPage.qml", {
                                "searchQuery": text})
                } else {
                    pageStack.replace("qrc:/LibraryPage.qml")
                }

                popup.close()
            }
        }
    }
    
    pageStack.anchors.bottomMargin: playerFooter.minimizedPlayerHeight + 1

    // media player
    PlayerFooter {
        id: playerFooter
        anchors.topMargin: -root.header.height
        anchors.fill: parent
        
        // only expand flicking area to full screen when it is open
        z: (contentY === 0) ? -1 : 999
    }
}
