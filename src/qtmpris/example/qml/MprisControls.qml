//SPDX-FileCopyrightText: 2015 Jolla Ltd. <valerio.valerio@jolla.com>
//SPDX-FileContributor: Andres Gomez
//
//SPDX-License-Identifier: LGPL-2.1-or-later


import QtQuick 2.0
import org.nemomobile.qtmpris 1.0

Item {
    id: controls

    property MprisManager mprisManager
    property bool isPlaying: mprisManager.currentService && mprisManager.playbackStatus == Mpris.Playing

    height: parent.height
    width: column.width

    Column {
        id: column

        Text {
            id: artistLabel

            text: if (mprisManager.currentService) {
                var artistTag = Mpris.metadataToString(Mpris.Artist)

                return (artistTag in mprisManager.metadata) ? mprisManager.metadata[artistTag].toString() : ""
            }
            width: parent.width
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
        }

        Text {
            id: songLabel

            text: if (mprisManager.currentService) {
                var titleTag = Mpris.metadataToString(Mpris.Title)

                return (titleTag in mprisManager.metadata) ? mprisManager.metadata[titleTag].toString() : ""
            }
            width: parent.width
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
        }

        Row {

            MouseArea {

                width: controls.parent.width * 0.25
                height: width

                onClicked: if (mprisManager.canGoPrevious) mprisManager.previous()

                Text {
                    anchors.centerIn: parent
                    text: "⏮"
                }
            }
            MouseArea {

                width: controls.parent.width * 0.25
                height: width

                onClicked: if ((controls.isPlaying && mprisManager.canPause) || (!controls.isPlaying && mprisManager.canPlay)) {
                    mprisManager.playPause()
                }

                Text {
                    anchors.centerIn: parent
                    text: controls.isPlaying ? "⏸" : "⏵"
                }
            }
            MouseArea {

                width: controls.parent.width * 0.25
                height: width

                onClicked: if (mprisManager.canGoPrevious) if (mprisManager.canGoNext) mprisManager.next()

                Text {
                    anchors.centerIn: parent
                    text: "⏭"
                }
            }
        }
    }
}

