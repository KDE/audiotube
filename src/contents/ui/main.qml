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

    footer: PlayerFooter {}
}
