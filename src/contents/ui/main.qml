import QtQuick 2.1
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.14 as Controls
import QtQuick.Layouts 1.3

import QtMultimedia 5.12

import org.kde.ytmusic 1.0

Kirigami.ApplicationWindow {
    id: root

    title: i18n("Youtube Music")

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }

    pageStack.initialPage: Kirigami.ScrollablePage {
        title: i18n("Youtube Music")

        header: Controls.Control {
            padding: Kirigami.Units.largeSpacing
            contentItem: Kirigami.ActionTextField {
                id: searchField
                rightActions: [
                    Kirigami.Action {
                        icon.name: "search"
                        onTriggered: searchModel.searchQuery = searchField.text
                    }
                ]
            }
        }

        ListView {
            model: SearchModel {
                id: searchModel

                onOpenAlbum: (browseId) => {
                    pageStack.push("qrc:/AlbumPage.qml", {
                        "browseId": browseId
                    })
                }

                onOpenArtist: (browseId) => {

                }

                onOpenPlaylist: (browseId) => {

                }

                onOpenSong: (videoId) => {
                    play(videoId)
                }

                onOpenVideo: (videoId) => {

                }
            }
            delegate: Kirigami.BasicListItem {
                required property int index
                required property string title
                required property int type

                text: title
                icon: {
                    switch (type) {
                    case SearchModel.Artist:
                        return "view-media-artist"
                    case SearchModel.Album:
                        return "media-album-cover"
                    case SearchModel.Playlist:
                        return "view-media-playlist"
                    case SearchModel.Song:
                        return "emblem-music-symbolic"
                    case SearchModel.Video:
                        return "emblem-videos-symbolic"
                    }
                }

                onClicked: searchModel.triggerItem(index)
            }
            Controls.BusyIndicator {
                anchors.centerIn: parent
                visible: searchModel.loading
            }
        }
    }

    function play(videoId) {
        footer.videoId = videoId
    }

    footer: ColumnLayout {
        property alias videoId: info.videoId
        property bool maximized: false

        anchors.left: parent.left
        anchors.right: parent.right
        height: maximized ? applicationWindow().height : player.preferredHeight

        Item {
            Layout.fillHeight: footer.maximized
            Layout.fillWidth: true
            visible: footer.maximized

            Kirigami.Heading {
                anchors.centerIn: parent
                text: "Nothing here yet"
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
            }

            Audio {
                id: audio
                source: info.audioUrl
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
}
