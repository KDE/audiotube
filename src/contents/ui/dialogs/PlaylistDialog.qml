// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

import org.kde.audiotube

Kirigami.Dialog {
    id: playlistsDialog

    title: i18n("Add Song to Playlist")

    preferredWidth: Kirigami.Units.gridUnit * 16

    property string videoId
    property string songTitle
    property string artists
    property string album: ""

    standardButtons: Kirigami.Dialog.NoButton
    customFooterActions: [
        Kirigami.Action {
            text: i18n("New Playlist")
            icon.name: "list-add"
            onTriggered: {
                addPlaylistDialog.open()
            }
        }
    ]

    ListView {

        Item {
            AddPlaylistDialog {
                id: addPlaylistDialog
                model: localPlaylistsModel
            }
        }

        reuseItems: true

        implicitHeight: 200
        model: LocalPlaylistsModel {
            id: localPlaylistsModel
        }
        delegate: Controls.ItemDelegate {
            required property string title
            required property string description
            required property int playlistId
            required property var thumbnailIds
            required property int index

            width: parent.width

            contentItem: RowLayout {
                Layout.fillHeight: true
                LocalPlaylistModel{id:localPlaylistModel}
                ThumbnailSource {
                    id: thumbnailSource1
                    videoId: thumbnailIds[0] ?? ""
                }
                ThumbnailSource {
                    id: thumbnailSource2
                    videoId: thumbnailIds[1] ?? thumbnailIds[0] ?? ""
                }
                ThumbnailSource {
                    id: thumbnailSource3
                    videoId: thumbnailIds[2] ?? thumbnailIds[0] ?? ""
                }
                ThumbnailSource {
                    id: thumbnailSource4
                    videoId: thumbnailIds[3] ?? thumbnailIds[0] ?? ""
                }
                PlaylistCover {
                    source1: thumbnailSource1.cachedPath
                    source2: thumbnailSource2.cachedPath
                    source3: thumbnailSource3.cachedPath
                    source4: thumbnailSource4.cachedPath
                    height: 35
                    width: height
                    radius: 5
                }

                ColumnLayout {
                    Controls.Label {
                        Layout.fillWidth: true
                        text: title
                        elide: Qt.ElideRight
                    }

                    Controls.Label {
                        Layout.fillWidth: true
                        visible: description
                        color: Kirigami.Theme.disabledTextColor
                        text: description
                        elide: Qt.ElideRight

                    }
                }
            }

            onClicked: {
                localPlaylistsModel.addPlaylistEntry(playlistId, videoId, songTitle, artists, album )
                playlistsDialog.close()
            }
        }
    }
}


