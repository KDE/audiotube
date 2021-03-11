import QtQuick 2.7
import QtQuick.Layouts 1.3

import QtQuick.Controls 2.12 as Controls
import org.kde.kirigami 2.12 as Kirigami

import org.kde.ytmusic 1.0

import QtMultimedia 5.12

ColumnLayout {
    id: footerLayout

    // input only
    property alias videoId: playlistModel.initialVideoId
    property alias playlistId: playlistModel.playlistId

    property bool maximized: false

    anchors.left: parent.left
    anchors.right: parent.right
    height: maximized ? applicationWindow().height : player.preferredHeight

    GridLayout {
        id: playerLayout
        Layout.fillHeight: footerLayout.maximized
        Layout.fillWidth: true

        flow: width > height ? GridLayout.LeftToRight : GridLayout.TopToBottom
        readonly property bool mobile: flow == GridLayout.TopToBottom

        visible: footerLayout.maximized
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: !playerLayout.mobile && playerLayout.width * 0.5
            Layout.preferredHeight: playerLayout.mobile && playerLayout.height * 0.5

            Image {
                source: info.thumbnail
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
            }
        }

        Controls.ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: !playerLayout.mobile && playerLayout.width * 0.5
            Layout.preferredHeight: playerLayout.mobile && playerLayout.height * 0.5
            visible: footerLayout.maximized

            ListView {
                id: playlistView

                model: PlaylistModel {
                    id: playlistModel
                }
                delegate: Kirigami.BasicListItem {
                    required property string title
                    required property string videoId
                    required property string artists
                    required property bool isCurrent

                    highlighted: isCurrent

                    onClicked: playlistModel.skipTo(videoId)

                    ColumnLayout {
                        anchors.fill: parent

                        Kirigami.Heading {
                            Layout.fillWidth: true

                            level: 2
                            text: title
                        }
                        Controls.Label {
                            Layout.fillWidth: true

                            text: artists
                        }
                    }
                }

                Controls.BusyIndicator {
                    anchors.centerIn: parent
                    visible: playlistModel.loading || playlistModel.loading
                }
            }
        }
    }
    Behavior on height {
        NumberAnimation {

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
            videoId: playlistModel.currentVideoId
        }

        Audio {
            id: audio
            source: info.audioUrl


            onStatusChanged: {
                if (status === Audio.EndOfMedia) {
                    console.log("Song ended")
                    playlistModel.next()
                }
            }
        }

        Controls.ToolButton {
            display: Controls.AbstractButton.IconOnly
            Layout.preferredWidth: parent.height
            Layout.fillHeight: true

            enabled: info.audioUrl != ""

            text: audio.playbackState === Audio.PlayingState ?  i18n("Pause") : i18n("Play")

            visible: !info.loading

            icon.name: audio.playbackState === Audio.PlayingState ? "media-playback-pause" : "media-playback-start"
            onClicked: audio.playbackState === Audio.PlayingState ? audio.pause() : audio.play()
        }

        Controls.ToolButton {
            display: Controls.AbstractButton.IconOnly
            Layout.preferredWidth: parent.height
            Layout.fillHeight: true

            enabled: playlistView.count > 1

            visible: !info.loading

            icon.name: "media-skip-forward"
            onClicked: playlistModel.next()
        }

        Controls.BusyIndicator {
            Layout.fillHeight: true
            visible: info.loading
        }

        ColumnLayout {
            Layout.fillHeight: true
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.bottomMargin: Kirigami.Units.largeSpacing

            Kirigami.Heading {
                level: 2
                text: info.title ? info.title : i18n("No media playing")
            }

            Controls.Slider {
                Layout.fillWidth: true

                from: 0
                to: audio.duration
                value: audio.position

                enabled: audio.seekable
                onMoved: {
                    console.log("Value:", value)
                    audio.seek(Math.floor(value))
                }

                Behavior on value {
                    NumberAnimation {

                    }
                }
            }
        }

        Controls.ToolButton {
            display: Controls.AbstractButton.IconOnly
            Layout.preferredWidth: parent.height

            text: i18n("Expand")

            Layout.fillHeight: true
            icon.name: footer.maximized ? "arrow-down" : "arrow-up"

            onClicked: footer.maximized = !footer.maximized
        }
    }
}
