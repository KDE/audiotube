import QtQuick 2.1
import org.kde.kirigami 2.14 as Kirigami

import org.kde.ytmusic 1.0

Kirigami.ApplicationWindow {
    id: root

    title: i18n("AudioTube")

    controlsVisible: !footer.maximized

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }

    pageStack.initialPage: "qrc:/SearchPage.qml"

    function play(videoId) {
        PlaylistModel.initialVideoId = videoId
    }

    function playPlaylist(playlistId) {
        PlaylistModel.playlistId = playlistId
    }

    function askPlayVideoAudio(videoId) {
        showPassiveNotification(i18n("Video playback is not supported yet."),
                            "long",
                            i18n("Play only audio"),
                            function() {
                                play(videoId)
                            })
    }

    Connections {
        target: ErrorHandler

        function onErrorOccurred(error) {
            showPassiveNotification(error)
        }
    }

    footer: PlayerFooter {}
}
