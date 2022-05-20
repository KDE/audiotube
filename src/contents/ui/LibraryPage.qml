import QtQuick 2.1
import QtQuick.Controls 2.12 as Controls
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.14 as Kirigami

import org.kde.ytmusic 1.0

Kirigami.ScrollablePage {
    ColumnLayout {
        Kirigami.Heading {
            text: i18n("Favourites")
        }
        Controls.ScrollView {
            Layout.fillWidth: true
            RowLayout {
                Repeater {
                    Layout.fillWidth: true
                    model: Library.favourites
                    onCountChanged: () => console.log("Count:", count)
                    delegate: ColumnLayout {
                        id: delegateItem
                        required property string title
                        required property string artist
                        required property string videoId

                        Layout.fillWidth: false
                        Layout.maximumWidth: 200
                        Kirigami.Card {
                            id: card
                            Layout.preferredHeight: 200

                            onClicked: play(delegateItem.videoId)

                            ThumbnailSource {
                                id: thumbnailSource
                                videoId: delegateItem.videoId
                            }

                            header: Image {
                                sourceSize: "200x200"
                                source: thumbnailSource.cachedPath
                                fillMode: Image.PreserveAspectCrop
                                asynchronous: true
                            }

                            actions: [
                                Kirigami.Action {
                                    icon.name: "delete"
                                    onTriggered: Library.removeFavourite(delegateItem.videoId)
                                }

                            ]
                        }
                        Controls.Label {
                            Layout.maximumWidth: 200
                            text: delegateItem.title
                            elide: Qt.ElideRight
                        }
                    }
                }
            }
        }
    }
}
