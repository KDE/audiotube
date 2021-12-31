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

    property alias searchField: searchField

    header: Controls.Control {
        visible: !footer.maximized
        padding: Kirigami.Units.largeSpacing

        background: Rectangle {
             width: parent.width
             height: parent.height
             anchors.fill: parent
             Kirigami.Theme.inherit: false
             Kirigami.Theme.colorSet: Kirigami.Theme.Header
             color:  Kirigami.Theme.backgroundColor
         }
        contentItem: RowLayout{
            Item {
                Layout.fillWidth: true
            }
            Kirigami.SearchField {
                id: searchField
                delaySearch: true
                selectByMouse: true
                Layout.fillWidth: true
                Layout.maximumWidth: 400
                onFocusChanged: {
                    if (focus)
                        popup.open()
                }
                onTextChanged: {
                    if (!popup.opened) {
                        popup.open()
                    }
                }

                Controls.Popup {
                    id: popup
                    x: searchField.y
                    y: searchField.y + searchField.height
                    visible: true
                    width: searchField.width
                    height: contentItem.implicitHeight

                    contentItem: Controls.ScrollView {
                        Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff
                        ListView {
                            id: completionList
                            model: Library.searches
                            delegate: Controls.ItemDelegate {
                                width: parent.width
                                text: modelData
                                onClicked: {
                                    searchField.text = modelData
                                    searchField.accepted()
                                }
                            }
                        }
                    }
                }

                onAccepted: {
                    pageStack.clear()
                    if (text) {
                        Library.addSearch(text)
                    }
                    pageStack.push("qrc:/SearchPage.qml", {
                               "searchQuery": text})
                    popup.close()
                }
            }
            Item {
                Layout.fillWidth: true
            }
        }
    }

    title: i18n("AudioTube")

    controlsVisible: !footer.maximized

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

    footer: PlayerFooter {}
}
