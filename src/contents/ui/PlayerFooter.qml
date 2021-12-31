// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12 as Controls
import org.kde.kirigami 2.12 as Kirigami
import org.kde.ytmusic 1.0
import QtMultimedia 5.12

Item {
    id: footerItem
    property bool maximized: false

    anchors.left: parent.left
    anchors.right: parent.right
    height: maximized ? applicationWindow().height : 75

    Rectangle {
        color: Kirigami.Theme.backgroundColor
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        anchors.fill: footerLayout
    }

    ColumnLayout {
        id: footerLayout
        height: parent.height
        anchors.fill: parent

        GridLayout {
            id: playerLayout

            readonly property bool mobile: width > height

            Layout.fillHeight: footerItem.maximized
            Layout.fillWidth: true
            flow: width > height ? GridLayout.LeftToRight : GridLayout.TopToBottom
            visible: footerItem.maximized

            ColumnLayout {
                Layout.fillWidth: info.thumbnail
                Layout.fillHeight: info.thumbnail
                Image {
                    source: info.thumbnail
                    Layout.preferredWidth: footerLayout.width * 0.5
                    fillMode: Image.PreserveAspectFit
                }
                Controls.ToolButton{
                    text: "Fav"
                    onClicked: Library.addFavourite(info.videoId)
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Controls.ScrollView {
                    anchors.fill: parent

                    ListView {
                        anchors.fill: parent
                        id: playlistView

                        clip: true

                        Controls.BusyIndicator {
                            anchors.centerIn: parent
                            visible: UserPlaylistModel.loading || UserPlaylistModel.loading
                        }

                        onCountChanged: {
                            if (count < 1) {
                                footerItem.maximized = false
                            }
                        }

                        model: UserPlaylistModel

                        delegate: Kirigami.SwipeListItem {
                            id: delegateItem
                            required property string title
                            required property string videoId
                            required property string artists
                            required property bool isCurrent

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

                                Controls.Label {
                                    elide: Text.ElideRight
                                    Layout.fillWidth: true
                                    text: artists
                                }
                            }

                            actions: [
                                Kirigami.Action {
                                    text: i18n("Remove Track")
                                    icon.name: "list-remove"
                                    onTriggered: UserPlaylistModel.remove(delegateItem.videoId)
                                }
                            ]
                        }

                        header: Controls.ToolBar {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            RowLayout {
                                anchors.fill: parent
                                Item {
                                    Layout.fillWidth: true
                                }

                                Controls.ToolButton {
                                    text: i18n("Clear")
                                    icon.name: "edit-clear-all"
                                    onClicked: UserPlaylistModel.clear()
                                }
                                Controls.ToolButton {
                                    text: i18n("Shuffle")
                                    icon.name: "media-playlist-shuffle"
                                    onClicked: UserPlaylistModel.shufflePlaylist()
                                }
                            }
                        }
                    }
                }
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
        }

        RowLayout {
            id: player

            Layout.fillWidth: true
            Layout.fillHeight: !footer.maximized

            VideoInfoExtractor {
                id: info

                onSongChanged: audio.play()
                videoId: UserPlaylistModel.currentVideoId
            }

            Audio {
                id: audio

                source: info.audioUrl
                onStatusChanged: {
                    if (status === Audio.EndOfMedia) {
                        console.log("Song ended");
                        UserPlaylistModel.next();
                    }
                }
            }

            Controls.ToolButton {
                display: Controls.AbstractButton.IconOnly
                Layout.preferredWidth: parent.height
                Layout.fillHeight: true
                enabled: info.audioUrl != ""
                text: audio.playbackState === Audio.PlayingState ? i18n("Pause") : i18n("Play")
                visible: !info.loading
                icon.name: audio.playbackState === Audio.PlayingState ? "media-playback-pause" : "media-playback-start"
                onClicked: audio.playbackState === Audio.PlayingState ? audio.pause() : audio.play()
            }

            Controls.ToolButton {
                display: Controls.AbstractButton.IconOnly
                Layout.preferredWidth: parent.height
                Layout.fillHeight: true
                enabled: UserPlaylistModel.canSkip
                visible: !info.loading
                icon.name: "media-skip-forward"
                onClicked: UserPlaylistModel.next()
            }

            Controls.BusyIndicator {
                Layout.fillHeight: true
                visible: info.loading
            }

            ColumnLayout {
                Layout.fillHeight: true
                Layout.topMargin: Kirigami.Units.smallSpacing
                Layout.bottomMargin: Kirigami.Units.smallSpacing

                Kirigami.Heading {
                    level: 3
                    Layout.fillWidth: true
                    elide: Qt.ElideRight
                    text: info.title ? info.title : i18n("No media playing")
                }

                RowLayout{

                    Controls.Label{
                        visible: info.title
                        text: PlayerUtils.formatTimestamp(audio.position)
                    }

                    Controls.Slider {
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

                    Controls.Label{
                        visible: info.title
                        text: PlayerUtils.formatTimestamp(audio.duration)
                    }

                }



            }

            Controls.ToolButton {
                display: Controls.AbstractButton.IconOnly
                Layout.preferredWidth: parent.height
                text: i18n("Expand")
                enabled: playlistView.count > 0
                Layout.fillHeight: true
                icon.name: footerItem.maximized ? "arrow-down" : "arrow-up"
                onClicked: footerItem.maximized = !footerItem.maximized
            }

        }
    }

    Behavior on height {
        NumberAnimation {
        }
    }
}
