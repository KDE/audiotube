// SPDX-FileCopyrightText: 2020-2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import QtGraphicalEffects 1.0
import QtMultimedia 5.12

import org.kde.kirigami 2.19 as Kirigami
import org.kde.ytmusic 1.0

Item {
    id: root
    
    required property var info // VideoInfoExtractor object
    required property var audio // Audio object
    
    signal requestClose()
    
    // background image
    Item {
        anchors.fill: parent
        
        Rectangle {
            anchors.fill: parent
            color: Qt.rgba(25, 25, 30, 1)
        }

        Image {
            anchors.fill: parent
            asynchronous: true
            
            source: info.thumbnail
            fillMode: Image.PreserveAspectCrop
            
            sourceSize.width: 512
            sourceSize.height: 512
        }
        
        layer.enabled: true
        layer.effect: HueSaturation {
            cached: true

            lightness: -0.5
            saturation: 1.9

            layer.enabled: true
            layer.effect: FastBlur {
                cached: true
                radius: 100
            }
        }
    }
    
    // content
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // hide arrow button
        ToolButton {
            id: closeButton
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.maximumHeight: parent.height
            Layout.preferredHeight: Kirigami.Units.gridUnit * 3
            Layout.maximumWidth: parent.height
            Layout.preferredWidth: Kirigami.Units.gridUnit * 3
            Layout.topMargin: Kirigami.Units.smallSpacing
            icon.name: "arrow-down"
            icon.color: "white"
            Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
            Kirigami.Theme.inherit: false
            onClicked: root.requestClose()
        }
        
        // top tab bar
        RowLayout {
            id: tabBar
            Layout.alignment: Qt.AlignHCenter
            
            ToolButton {
                id: nowPlayingTab
                text: i18n("Now Playing")
                padding: Kirigami.Units.largeSpacing
                onClicked: swipeView.setCurrentIndex(0)
                Layout.preferredHeight: Kirigami.Units.gridUnit * 2
                Layout.preferredWidth: Kirigami.Units.gridUnit * 9
                
                contentItem: Label {
                    text: nowPlayingTab.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    color: "white" // we need to ensure the label is white
                }
                
                // selection indicator
                Rectangle {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    color: Kirigami.Theme.highlightColor
                    height: 1
                    visible: swipeView.currentIndex == 0
                }
            }
            ToolButton {
                id: queueTab
                text: i18n("Queue")
                padding: Kirigami.Units.largeSpacing
                onClicked: swipeView.setCurrentIndex(1)
                Layout.preferredHeight: Kirigami.Units.gridUnit * 2
                Layout.preferredWidth: Kirigami.Units.gridUnit * 9
                
                contentItem: Label {
                    text: queueTab.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    color: "white"
                }
                
                // selection indicator
                Rectangle {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    color: Kirigami.Theme.highlightColor
                    height: 1
                    visible: swipeView.currentIndex == 1
                }
            }
        }
        
        // tabs
        SwipeView {
            id: swipeView
            property double specWidth: {
                let allowedWidth = root.width - Kirigami.Units.largeSpacing * 4;
                let allowedHeight = root.height - Kirigami.Units.largeSpacing * 8 - (closeButton.height + tabBar.height + bottomPlayerControls.height);
                if (allowedWidth > allowedHeight) {
                    return allowedHeight;
                } else {
                    return allowedWidth;
                }
            }
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.fillWidth: true
            Layout.preferredHeight: specWidth
        
            // music art
            Item {
                height: swipeView.width
                width: swipeView.height

                Image {
                    anchors.centerIn: parent
                    width: swipeView.specWidth
                    height: swipeView.specWidth

                    asynchronous: true
                    mipmap: true

                    source: info.thumbnail

                    sourceSize {
                        width: 512
                        height: 512
                    }

                    fillMode: Image.PreserveAspectCrop
                }
            }
            
            // playlist
            ListView {
                id: playlistView
                width: swipeView.width
                height: swipeView.height
                clip: true
                
                Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                Kirigami.Theme.inherit: false

                BusyIndicator {
                    anchors.centerIn: parent
                    visible: UserPlaylistModel.loading || UserPlaylistModel.loading
                }

                model: UserPlaylistModel

                delegate: Kirigami.SwipeListItem {
                    id: delegateItem
                    required property string title
                    required property string videoId
                    required property string artists
                    required property bool isCurrent

                    backgroundColor: 'transparent'
                    highlighted: isCurrent
                    onClicked: UserPlaylistModel.skipTo(videoId)

                    ColumnLayout {
                        Layout.fillWidth: true
                        Kirigami.Heading {
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                            level: 2
                            text: title
                        }

                        Label {
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                            text: artists
                        }
                    }

                    actions: [
                        Kirigami.Action {
                            text: i18n("Remove Track")
                            icon.name: "list-remove"
                            icon.color: "white"
                            onTriggered: UserPlaylistModel.remove(delegateItem.videoId)
                        }
                    ]
                }
            }
        }
        
        ColumnLayout {
            id: bottomPlayerControls
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.leftMargin: Kirigami.Units.gridUnit * 2
            Layout.rightMargin: Kirigami.Units.gridUnit * 2
            Layout.bottomMargin: Kirigami.Units.gridUnit * 2
            
            // song name
            Label {
                id: mainLabel
                text: info.title ? info.title : i18n("No media playing")
                
                Layout.fillWidth: true
                
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
                maximumLineCount: 1
                // Hardcoded because the footerbar blur always makes a dark-ish
                // background, so we don't want to use a color scheme color that
                // might also be dark
                color: "white"
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.5
                font.weight: Font.Bold
                font.bold: true
            }
            
            // song artist
            Kirigami.Heading {
                id: authorLabel
                text: info.artist ? info.artist : info.channel
                
                Layout.fillWidth: true
                
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
                maximumLineCount: 1
                // Hardcoded because the footerbar blur always makes a dark-ish
                // background, so we don't want to use a color scheme color that
                // might also be dark
                color: "white"
                opacity: 0.9
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.3
                font.weight: Font.Bold
                font.bold: true
            }
            
            // slider row
            RowLayout {
                Layout.topMargin: Kirigami.Units.gridUnit * 2
                spacing: Kirigami.Units.smallSpacing
                
                Label {
                    Layout.alignment: Qt.AlignVCenter
                    color: "white"
                    visible: info.title
                    text: PlayerUtils.formatTimestamp(audio.position)
                }

                Slider {
                    Layout.alignment: Qt.AlignVCenter
                    Layout.fillWidth: true
                    from: 0
                    to: audio.duration
                    value: audio.position
                    enabled: audio.seekable
                    onMoved: {
                        console.log("Value:", value);
                        audio.seek(Math.floor(value));
                    }

                    Behavior on value {
                        NumberAnimation {
                            duration: 1000
                        }
                    }
                }

                Label {
                    Layout.alignment: Qt.AlignVCenter
                    color: "white"
                    visible: info.title
                    text: PlayerUtils.formatTimestamp(audio.duration)
                }
            }
            
            RowLayout {
                Layout.topMargin: Kirigami.Units.largeSpacing
                Layout.alignment: Qt.AlignCenter
                Layout.fillWidth: true
                spacing: Kirigami.Units.largeSpacing
                
                // ensure white icons
                Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                Kirigami.Theme.inherit: false

                ToolButton {
                    id: favouriteButton
                    readonly property QtObject favouriteWatcher: Library.favouriteWatcher(info.videoId)
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 2.5
                    Layout.maximumWidth: height
                    Layout.preferredWidth: height
                    
                    onClicked: if (favouriteWatcher)
                                   favouriteWatcher.isFavourite
                                    ? Library.removeFavourite(info.videoId)
                                    : Library.addFavourite(info.videoId, info.title)
                    
                    icon.name: favouriteWatcher ? (favouriteWatcher.isFavourite ? "starred-symbolic" : "non-starred-symbolic") : ""
                    icon.width: Kirigami.Units.gridUnit * 1.5
                    icon.height: Kirigami.Units.gridUnit * 1.5
                    icon.color: "white"
                    
                    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                    Kirigami.Theme.inherit: false
                }

                Kirigami.PromptDialog {
                    id: playlistsDialog

                    property string videoId

                    mainItem: ListView {
                        implicitHeight: 200
                        model: LocalPlaylistsModel {
                        }

                        delegate: Kirigami.BasicListItem {
                            required property string title
                            required property int playlistId

                            text: title

                            onClicked: Library.addPlaylistEntry(playlistId, playlistsDialog.videoId, info.title)
                        }
                    }
                }

                ToolButton {
                    id: addToPlaylistButton
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 2.5
                    Layout.maximumWidth: height
                    Layout.preferredWidth: height

                    onClicked: {
                        playlistsDialog.videoId = info.videoId
                        playlistsDialog.open()
                    }

                    icon.name: "media-playlist-append"
                    icon.width: Kirigami.Units.gridUnit * 1.5
                    icon.height: Kirigami.Units.gridUnit * 1.5
                    icon.color: "white"

                    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                    Kirigami.Theme.inherit: false
                }
                
                ToolButton {
                    id: skipBackwardButton
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 2.5
                    Layout.maximumWidth: height
                    Layout.preferredWidth: height
                    
                    enabled: false
                    
                    icon.name: "media-skip-backward"
                    icon.width: Kirigami.Units.gridUnit * 1.5
                    icon.height: Kirigami.Units.gridUnit * 1.5
                    icon.color: "white"
                    
                    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                    Kirigami.Theme.inherit: false
                }
                
                ToolButton {
                    id: playPauseButton
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 4
                    Layout.maximumWidth: height
                    Layout.preferredWidth: height
                    
                    enabled: info.audioUrl != ""
                    onClicked: audio.playbackState === Audio.PlayingState ? audio.pause() : audio.play()
                    
                    icon.name: audio.playbackState === Audio.PlayingState ? "media-playback-pause" : "media-playback-start"
                    icon.width: Kirigami.Units.gridUnit * 3
                    icon.height: Kirigami.Units.gridUnit * 3
                    icon.color: "white"
                    
                    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                    Kirigami.Theme.inherit: false
                }
                
                ToolButton {
                    id: skipForwardButton
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 2.5
                    Layout.maximumWidth: height
                    Layout.preferredWidth: height
                    
                    enabled: UserPlaylistModel.canSkip
                    onClicked: UserPlaylistModel.next()
                    
                    icon.name: "media-skip-forward"
                    icon.width: Kirigami.Units.gridUnit * 1.5
                    icon.height: Kirigami.Units.gridUnit * 1.5
                    icon.color: "white"
                    
                    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                    Kirigami.Theme.inherit: false
                }
                
                ToolButton {
                    id: shuffleButton
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 2.5
                    Layout.maximumWidth: height
                    Layout.preferredWidth: height
                    
                    onClicked: UserPlaylistModel.shufflePlaylist()
                    
                    icon.name: "media-playlist-shuffle"
                    icon.width: Kirigami.Units.gridUnit * 1.5
                    icon.height: Kirigami.Units.gridUnit * 1.5
                    icon.color: "white"
                    
                    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                    Kirigami.Theme.inherit: false
                }
            }
        }
    }
}
