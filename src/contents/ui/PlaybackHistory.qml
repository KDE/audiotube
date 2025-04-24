// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami

import org.kde.audiotube

Kirigami.ScrollablePage {
    id: playbackHistoryPage
    title: i18n("Unknown list of songs")
    property QtObject dataModel: (objectName == "history") ? Library.playbackHistory : (objectName == "favourites" ? Library.favourites : null)

    DoubleActionButton {
        id: action
        visible: false
        property bool shown
        shown: !playbackHistoryPage.flickable.atYBeginning
        onShownChanged:
            if(shown){
                visible = true
                appear.running = true
            } else {
                disappear.running = true
            }

        parent: overlay
        x: playbackHistoryPage.width - width - margin
        y: playbackHistoryPage.height - height - margin
        NumberAnimation on y {
            id: appear
            easing.type: Easing.InCubic
            running: false
            from: playbackHistoryPage.height
            to: playbackHistoryPage.height - action.height - action.margin
            duration: 100
        }
        NumberAnimation on y {
            id: disappear
            easing.type: Easing.OutCubic
            running: false
            from: playbackHistoryPage.height - action.height - action.margin
            to: playbackHistoryPage.height
            duration: 100
            onFinished: action.visible = false
        }
        rightAction: Kirigami.Action {
            icon.name: "media-playlist-shuffle"
            text: i18n("Shuffle")
            onTriggered: {
                if(playbackHistoryPage.objectName == "favourites") {
                    UserPlaylistModel.playFavourites(Library.favourites, true)
                }
                else if(playbackHistoryPage.objectName == "history") {
                    onClicked: UserPlaylistModel.playPlaybackHistory(playbackHistoryPage.dataModel, true)
                }
            }
        }
        leftAction: Kirigami.Action {
            icon.name: "media-playback-start"
            text: i18n("Play")
            onTriggered: {
                if(playbackHistoryPage.objectName == "favourites") {
                    UserPlaylistModel.playFavourites(Library.favourites, false)
                }
                else if(playbackHistoryPage.objectName == "history") {
                    onClicked: UserPlaylistModel.playPlaybackHistory(playbackHistoryPage.dataModel, false)
                }
            }
        }

    }

    SongMenu {
        id: menu
    }
    ListView {
        footer: Item { height: 60 }
        header: RowLayout {
            spacing: Kirigami.Units.mediumSpacing

            Controls.ToolButton {
                text: i18n("Play")
                icon.name: "media-playback-start"
                onClicked: {
                    if(playbackHistoryPage.objectName == "favourites") {
                        UserPlaylistModel.playFavourites(Library.favourites, false)
                    }
                    else if(playbackHistoryPage.objectName == "history") {
                        onClicked: UserPlaylistModel.playPlaybackHistory(playbackHistoryPage.dataModel, false)
                    }
                }
            }

            Controls.ToolButton {
                text: i18n("Shuffle")
                icon.name: "shuffle"
                onClicked: {
                    if(playbackHistoryPage.objectName == "favourites") {
                        UserPlaylistModel.playFavourites(Library.favourites, true)
                    }
                    else if(playbackHistoryPage.objectName == "history") {
                        onClicked: UserPlaylistModel.playPlaybackHistory(playbackHistoryPage.dataModel, true)
                    }
                }
            }
            Controls.ToolButton {
                text: i18n("Append to queue")
                icon.name: "media-playlist-append"
                onClicked: {
                    if(playbackHistoryPage.objectName == "favourites") {
                        UserPlaylistModel.appendFavourites(Library.favourites,false)
                    }
                    else if(playbackHistoryPage.objectName == "history") {
                        UserPlaylistModel.appendPlaybackHistory(playbackHistoryPage.dataModel, false)
                    }
                }
            }
            Item {
                Layout.fillWidth: true
            }
        }

        id: listView
        reuseItems: true

        Kirigami.PlaceholderMessage {
            visible: listView.count < 1
            text: i18n("No songs here yet")
            anchors.centerIn: parent
        }

        model: playbackHistoryPage.dataModel

        delegate: Controls.ItemDelegate {
            id: delegateItem

            required property string title
            required property string videoId
            required property var artists
            required property string artistsDisplayString
            required property int index

            width: parent.width

            contentItem: MouseArea {
                implicitHeight: content.implicitHeight
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: mouse => {
                    if (mouse.button === Qt.RightButton) {
                        menu.openForSong(delegateItem.videoId, delegateItem.title, delegateItem.artists, delegateItem.artistsDisplayString)
                    } else if (mouse.button === Qt.LeftButton) {
                        play(videoId)
                    }
                }
                RowLayout {
                    id: content
                    anchors.fill: parent
                    ThumbnailSource {
                        id: thumbnailSource

                        videoId: delegateItem.videoId
                    }

                    RoundedImage {
                        source: thumbnailSource.cachedPath
                        height: 35
                        width: height
                        radius: 5
                    }
                    ColumnLayout {
                        Controls.Label {
                            text: delegateItem.title
                            Layout.fillWidth: true
                            elide: Qt.ElideRight

                        }
                        Controls.Label {
                            Layout.fillWidth: true
                            color: Kirigami.Theme.disabledTextColor
                            text: delegateItem.artistsDisplayString
                            elide: Qt.ElideRight

                        }
                    }

                    Controls.ToolButton {
                        icon.name: "overflow-menu"
                        text: i18n("More")
                        display: Controls.AbstractButton.IconOnly
                        onClicked: menu.openForSong(delegateItem.videoId, delegateItem.title, delegateItem.artists, delegateItem.artistsDisplayString)
                    }
                }
            }
        }
    }
}
