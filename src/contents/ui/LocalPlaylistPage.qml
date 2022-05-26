import QtQuick 2.15

import org.kde.kirigami 2.19 as Kirigami

import org.kde.ytmusic 1.0

Kirigami.ScrollablePage {
    property alias playlistId: playlistModel.playlistId

    ListView {
        Kirigami.PlaceholderMessage {
            text: i18n("This playlist is still empty")
            anchors.centerIn: parent
            visible: parent.count < 1
        }

        model: LocalPlaylistModel {
            id: playlistModel
        }

        delegate: Kirigami.BasicListItem {
            required property string title

            text: title
        }
    }
}
