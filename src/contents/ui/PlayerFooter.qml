// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12 as Controls
import org.kde.kirigami 2.12 as Kirigami
import org.kde.ytmusic 1.0
import QtMultimedia 5.12

ColumnLayout {
    id: footerLayout

    property bool maximized: false

    anchors.left: parent.left
    anchors.right: parent.right
    height: maximized ? applicationWindow().height : player.preferredHeight

    GridLayout {
        id: playerLayout

        readonly property bool mobile: width > height

        Layout.fillHeight: footerLayout.maximized
        Layout.fillWidth: true
        flow: width > height ? GridLayout.LeftToRight : GridLayout.TopToBottom
        visible: footerLayout.maximized

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Image {
                anchors.fill: parent

                source: info.thumbnail
                fillMode: Image.PreserveAspectFit
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
                            footerLayout.maximized = false
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
                                text: i18n("Remove track")
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
                                icon.name: "edit-clear-all"
                                onClicked: UserPlaylistModel.clear()
                            }
                            Controls.ToolButton {
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
                    visible: audio.duration > 0
                    text: {
                        var hours = new Date(audio.position).getUTCHours()
                        if(hours>0)
                            hours = (("0"+hours).slice(-2)) + ":"
                        else
                            hours = ""

                        var minutes = new Date(audio.position).getUTCMinutes()
                        minutes = ("0"+minutes).slice(-2)

                        var seconds = new Date(audio.position).getUTCSeconds()
                        seconds = ("0"+seconds).slice(-2)

                        return hours+minutes+":"+seconds;
                    }
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
                        }

                    }

                }

                Controls.Label{
                    visible: audio.duration > 0
                    text: {
                        var hours = new Date(audio.duration).getUTCHours()
                        if(hours>0)
                            hours = (("0"+hours).slice(-2)) + ":"
                        else
                            hours = ""

                        var minutes = new Date(audio.duration).getUTCMinutes()
                        minutes = ("0"+minutes).slice(-2)

                        var seconds = new Date(audio.duration).getUTCSeconds()
                        seconds = ("0"+seconds).slice(-2)

                        return hours+minutes+":"+seconds;
                    }
                }

            }



        }

        Controls.ToolButton {
            display: Controls.AbstractButton.IconOnly
            Layout.preferredWidth: parent.height
            text: i18n("Expand")
            enabled: playlistView.count > 0
            Layout.fillHeight: true
            icon.name: footerLayout.maximized ? "arrow-down" : "arrow-up"
            onClicked: footerLayout.maximized = !footerLayout.maximized
        }

    }

    Behavior on height {
        NumberAnimation {
        }

    }

}
