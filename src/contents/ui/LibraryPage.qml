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
                        Layout.fillWidth: false
                        Layout.maximumWidth: 200
                        Component.onCompleted: {
                            console.log(modelData)
                        }
                        Kirigami.Card {
                            id: card
                            Layout.preferredHeight: 200

                            onClicked: play(modelData)

                            ThumbnailSource {
                                id: thumbnailSource
                                videoId: modelData
                            }

                            header: Image {
                                sourceSize: "200x200"
                                source: thumbnailSource.cachedPath
                                fillMode: Image.PreserveAspectCrop
                                asynchronous: true
                            }
                        }
                        Controls.Label {
                            Layout.maximumWidth: 200
                            text: Library.videoTitle(modelData)
                            elide: Qt.ElideRight
                        }
                    }
                }
            }
        }
    }
}
