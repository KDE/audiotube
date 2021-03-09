import QtQuick 2.1
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.14 as Controls
import QtQuick.Layouts 1.3

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

                }

                onOpenVideo: (videoId) => {

                }
            }
            delegate: Kirigami.BasicListItem {
                required property int index
                required property string display

                text: display
                onClicked: searchModel.triggerItem(index)
            }
            Controls.BusyIndicator {
                anchors.centerIn: parent
                visible: searchModel.loading
            }
        }
    }
}
