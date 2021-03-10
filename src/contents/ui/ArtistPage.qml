import QtQuick 2.1
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.14 as Controls
import QtQuick.Layouts 1.3

import org.kde.ytmusic 1.0

Kirigami.ScrollablePage {
    property alias channelId: artistModel.channelId

    title: artistModel.title

    ListView {
        model: ArtistModel {
            id: artistModel

            onOpenAlbum: (browseId) => {
                pageStack.push("qrc:/AlbumPage.qml", {
                    "browseId": browseId
                })
            }

            onOpenSong: (videoId) => {
                play(videoId)
            }

            onOpenVideo: (videoId) => {

            }
        }
        section.property: "type"
        section.delegate: Kirigami.ListSectionHeader {
            text: {
                switch(parseInt(section)) {
                case ArtistModel.Album:
                    return i18n("Albums")
                case ArtistModel.Single:
                    return i18n("Singles")
                case ArtistModel.Song:
                    return i18n("Songs")
                case ArtistModel.Video:
                    return i18n("Videos")
                }
            }
        }
        delegate: Kirigami.BasicListItem {
            required property string title
            required property int type
            required property int index

            text: title
            icon: {
                switch (type) {
                case ArtistModel.Album:
                    return "media-album-cover"
                case ArtistModel.Single:
                case ArtistModel.Song:
                    return "emblem-music-symbolic"
                case ArtistModel.Video:
                    return "emblem-videos-symbolic"
                }
            }
            onClicked: artistModel.triggerItem(index)
        }

        Controls.BusyIndicator {
            anchors.centerIn: parent
            visible: artistModel.loading
        }
    }
}
