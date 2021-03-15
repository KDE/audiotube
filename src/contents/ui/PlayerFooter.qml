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

            ListView {
                anchors.fill: parent
                id: playlistView

                clip: true

                Controls.BusyIndicator {
                    anchors.centerIn: parent
                    visible: PlaylistModel.loading || PlaylistModel.loading
                }

                onCountChanged: {
                    if (count < 1) {
                        footerLayout.maximized = false
                    }
                }

                model: PlaylistModel

                delegate: Kirigami.SwipeListItem {
                    id: delegateItem
                    required property string title
                    required property string videoId
                    required property string artists
                    required property bool isCurrent

                    highlighted: isCurrent
                    onClicked: PlaylistModel.skipTo(videoId)

                    ColumnLayout {
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

                    actions: [
                        Kirigami.Action {
                            text: i18n("Remove track")
                            icon.name: "list-remove"
                            onTriggered: PlaylistModel.remove(delegateItem.videoId)
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
                            onClicked: PlaylistModel.clear()
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
            videoId: PlaylistModel.currentVideoId
        }

        Audio {
            id: audio

            source: info.audioUrl
            onStatusChanged: {
                if (status === Audio.EndOfMedia) {
                    console.log("Song ended");
                    PlaylistModel.next();
                }
            }
        }

        Controls.ToolButton {
            display: Controls.AbstractButton.IconOnly
            Layout.preferredWidth: parent.height
            Layout.fillHeight: true
            enabled: info.audioUrl !== ""
            text: audio.playbackState === Audio.PlayingState ? i18n("Pause") : i18n("Play")
            visible: !info.loading
            icon.name: audio.playbackState === Audio.PlayingState ? "media-playback-pause" : "media-playback-start"
            onClicked: audio.playbackState === Audio.PlayingState ? audio.pause() : audio.play()
        }

        Controls.ToolButton {
            display: Controls.AbstractButton.IconOnly
            Layout.preferredWidth: parent.height
            Layout.fillHeight: true
            enabled: PlaylistModel.canSkip
            visible: !info.loading
            icon.name: "media-skip-forward"
            onClicked: PlaylistModel.next()
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
