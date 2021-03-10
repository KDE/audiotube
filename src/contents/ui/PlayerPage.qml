import QtQuick 2.1
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.14 as Controls
import QtQuick.Layouts 1.3

import QtMultimedia 5.12

import org.kde.ytmusic 1.0

Kirigami.ScrollablePage {
    property alias videoId: info.videoId
    title: info.title

    Audio {
        id: audio
        source: info.audioUrl
    }

    Controls.Button {
        text: i18n("Play")
        onClicked: audio.play()
    }

    VideoInfoExtractor {
        id: info
    }
}
