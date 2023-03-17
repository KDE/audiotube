//SPDX-FileCopyrightText: 2015 Jolla Ltd. <valerio.valerio@jolla.com>
//SPDX-FileContributor: Andres Gomez
//
//SPDX-License-Identifier: LGPL-2.1-or-later


import QtQuick 2.0
import org.nemomobile.qtmpris 1.0

Item {
    id: mainItem

    anchors.fill: parent

    Column {
        id: column

        x: (parent.width - width) * 0.5
        width: parent.width * 0.75

        Row {
            MouseArea {
                width: column.parent.width * 0.25
                height: width

                onClicked: mprisPlayer.playbackStatus = Mpris.Stopped

                Text {
                    anchors.centerIn: parent
                    text: "Stop"
                }
            }
            MouseArea {
                width: column.parent.width * 0.25
                height: width

                onClicked: mprisPlayer.playbackStatus = Mpris.Playing

                Text {
                    anchors.centerIn: parent
                    text: "Play"
                }
            }
            MouseArea {
                width: column.parent.width * 0.25
                height: width

                onClicked: mprisPlayer.playbackStatus = Mpris.Paused

                Text {
                    anchors.centerIn: parent
                    text: "Pause"
                }
            }
        }

        Row {

            width: parent.width
            height: artistSet.height

            Text {
                id: artistLabel

                height: parent.height
                text: "Artist: "
                verticalAlignment: Text.AlignVCenter
            }

            TextInput {
                id: artistInput

                width: parent.width - artistLabel.width - artistSet.width
                height: parent.height
                verticalAlignment: Text.AlignVCenter
            }

            MouseArea {
                id: artistSet

                width: column.parent.width * 0.25
                height: width

                onClicked: mprisPlayer.artist = artistInput.text

                Text {
                    anchors.centerIn: parent
                    text: "Set"
                }
            }
        }

        Row {

            width: parent.width
            height: songSet.height

            Text {
                id: songLabel

                height: parent.height
                text: "Song: "
                verticalAlignment: Text.AlignVCenter
            }

            TextInput {
                id: songInput

                width: parent.width - songLabel.width - songSet.width
                height: parent.height
                verticalAlignment: Text.AlignVCenter
            }

            MouseArea {
                id: songSet

                width: column.parent.width * 0.25
                height: width

                onClicked: mprisPlayer.song = songInput.text

                Text {
                    anchors.centerIn: parent
                    text: "Set"
                }
            }
        }

        Text {
            id: message

            property string lastMessage

            text: "Last Message was: " + lastMessage
            width: parent.width
            elide: Text.ElideRight
        }
    }

    MprisPlayer {
        id: mprisPlayer

        property string artist
        property string song

        serviceName: "qtmpris"

        // Mpris2 Root Interface
        identity: "QtMpris Example"
        supportedUriSchemes: ["file"]
        supportedMimeTypes: ["audio/x-wav", "audio/x-vorbis+ogg"]

        // Mpris2 Player Interface
        canControl: true

        canGoNext: true
        canGoPrevious: true
        canPause: playbackStatus == Mpris.Playing
        canPlay: playbackStatus != Mpris.Playing
        canSeek: false

        playbackStatus: Mpris.Stopped
        loopStatus: Mpris.None
        shuffle: false
        volume: 1

        onPauseRequested: message.lastMessage = "Pause requested"
        onPlayRequested: message.lastMessage = "Play requested"
        onPlayPauseRequested: message.lastMessage = "Play/Pause requested"
        onStopRequested: message.lastMessage = "Stop requested"
        onNextRequested: message.lastMessage = "Next requested"
        onPreviousRequested: message.lastMessage = "Previous requested"
        onSeekRequested: {
            message.lastMessage = "Seeked requested with offset - " + offset + " microseconds"
            emitSeeked()
        }
        onSetPositionRequested: {
            message.lastMessage = "Position requested to - " + position + " microseconds"
            emitSeeked()
        }
        onOpenUriRequested: message.lastMessage = "Requested to open uri \"" + url + "\""

        onLoopStatusRequested: {
            if (loopStatus == Mpris.None) {
                repeatSwitch.checked = false
            } else if (loopStatus == Mpris.Playlist) {
                repeatSwitch.checked = true
            }
        }
        onShuffleRequested: shuffleSwitch.checked = shuffle

        onArtistChanged: {
            var metadata = mprisPlayer.metadata

            metadata[Mpris.metadataToString(Mpris.Artist)] = [artist] // List of strings

            mprisPlayer.metadata = metadata
        }

        onSongChanged: {
            var metadata = mprisPlayer.metadata

            metadata[Mpris.metadataToString(Mpris.Title)] = song // String

            mprisPlayer.metadata = metadata
        }
    }
}
