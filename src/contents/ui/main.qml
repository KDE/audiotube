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

    // so that there is still a separator, since the header style is none
    Kirigami.Separator {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        visible: pageStack.currentItem.objectName == "libraryPage"
                 && pageStack.layers.depth == 1
    }

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
    pageStack.clip: true

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
        id: searchAlbum

        ColumnLayout {
            Layout.alignment: Qt.AlignTop
            id: mpdelegateItem

            width: 100
            Layout.maximumWidth: 70

            Kirigami.ShadowedRectangle {
                id: recCover
                MouseArea {
                    id: recArea
                    anchors.fill: parent
                    onClicked: play(model.videoId)
                    hoverEnabled: !Kirigami.Settings.hasTransientTouchInput
                    onEntered: {
                        if (!Kirigami.Settings.hasTransientTouchInput)
                            recSelected.visible = true
                    }
                    onExited: recSelected.visible = false

                }
                Layout.margins: 5

                width: 70
                height: 70
                radius: 10
                shadow.size: 15
                shadow.xOffset: 5
                shadow.yOffset: 5
                shadow.color: Qt.rgba(0, 0, 0, 0.2)
                Rectangle {
                    width: 70
                    height: 70

                    color: "transparent"

                    ThumbnailSource {
                        id: thumbnailSource
                        videoId: model ? model.videoId : ""
                    }
                    RoundedImage {
                        source: thumbnailSource.cachedPath
                        height: parent.height
                        width: height
                        radius: 10
                    }
                    Rectangle {
                        id: recSelected

                        Rectangle {
                            anchors.fill: parent
                            color: Kirigami.Theme.hoverColor
                            radius: 10
                            opacity: 0.2
                        }

                        visible: false
                        anchors.fill: parent

                        radius: 9

                        border.color: Kirigami.Theme.hoverColor
                        border.width: 2
                        color: "transparent"
                    }
                }
            }
            Controls.Label {
                leftPadding:5
                Layout.maximumWidth: 70
                text: model ? model.title : ""
                elide: Qt.ElideRight
                wrapMode: Text.WordWrap
                Layout.maximumHeight: 40
            }
            Item {
                height: 5
            }
        }
    }
    
    Component {
        id: searchFieldComponent
        
        Kirigami.SearchField {
            id: searchField

            autoAccept: false
            selectByMouse: true
            onPressed: {
                popup.open()
            }

            property alias popup: popup

            property var filterExpression: new RegExp(`.*${searchField.text}.*`, "i")

            Controls.Popup {
                padding: 1
                id: popup
                x: searchField.y
                y: searchField.y + searchField.height
                width: searchField.width
                height: completionList
                        ? Math.min(completionList.count * Kirigami.Units.gridUnit * 2 + Kirigami.Units.gridUnit * 2 + recents.implicitHeight, Kirigami.Units.gridUnit * 20)
                        : Kirigami.Units.gridUnit * 20

                contentItem: Controls.ScrollView {
                    contentWidth: -1
                    ColumnLayout {
                        width: popup.width

                        Controls.ScrollView {
                            id: recents
                            Layout.fillWidth: true
                            Layout.maximumWidth: popup.width - 23

                            visible: searchField.text && recentsRepeater.count > 0

                            leftPadding: 10
                            topPadding: 10
                            Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff
                            RowLayout {
                                spacing: 10
                                Layout.fillWidth: true
                                Layout.maximumWidth: popup.width - 23

                                Repeater {
                                    id: recentsRepeater
                                    Layout.fillWidth: true
                                    model: SortFilterModel {
                                        filterRole: PlaybackHistoryModel.Title
                                        filterRegularExpression: searchField.filterExpression
                                        sourceModel: Library.playbackHistory
                                    }
                                    delegate: Kirigami.DelegateRecycler {
                                        sourceComponent: searchAlbum
                                    }
                                }
                            }
                        }
                        Kirigami.Separator{
                            Layout.fillWidth: true
                        }
                        Repeater {
                            Layout.fillWidth: true

                            id: completionList
                            model: SortFilterModel {
                                sourceModel: Library.searches
                                filterRegularExpression: searchField.filterExpression
                            }

                            delegate: Controls.ItemDelegate {
                                Kirigami.Theme.colorSet: Kirigami.Theme.Window
                                Kirigami.Theme.inherit: false
                                Layout.fillWidth: true

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
            }

            onAccepted: {
                while (pageStack.depth > 0) {
                    pageStack.pop()
                }

                pageStack.layers.clear()

                if (text) {
                    Library.addSearch(text)
                    pageStack.push("qrc:/SearchPage.qml", {
                                "searchQuery": text})
                } else {
                    pageStack.push("qrc:/LibraryPage.qml")
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
