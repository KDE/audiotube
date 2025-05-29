// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
// SPDX-FileCopyrightText: 2021 Bart De Vries <bart@mogwai.be>
// SPDX-FileCopyrightText: 2020-2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Effects
import QtQuick.Controls as Controls

import QtMultimedia

import org.kde.kirigami as Kirigami
import org.kde.audiotube

Flickable {
    id: footerItem

    property int footerSpacing: 0
    property bool maximized: false

    readonly property int progressBarHeight: Kirigami.Units.gridUnit / 4
    readonly property int minimizedPlayerContentHeight: Math.round(Kirigami.Units.gridUnit * 3.5)
    readonly property int minimizedPlayerHeight: minimizedPlayerContentHeight + progressBarHeight

    readonly property string thumbnail: thumbnailSource.cachedPath

    boundsBehavior: Flickable.StopAtBounds

    function close() {
        toClose.restart();
    }

    function resetToBoundsOnFlick() {
        if (!atYBeginning || !atYEnd) {
            if (footerItem.verticalVelocity > 0) {
                toOpen.restart();
            } else if (footerItem.verticalVelocity < 0) {
                toClose.restart();
            } else { // i.e. when verticalVelocity === 0
                if (contentY > contentHeight / 4) {
                    toOpen.restart();
                } else  {
                    toClose.restart();
                }
            }
        }
    }

    function resetToBoundsOnResize() {
        if (contentY > contentHeight / 4) {
            contentY = contentHeight / 2;
        } else {
            contentY = 0;
        }
    }

    NumberAnimation on contentY {
        id: toOpen
        from: contentY
        to: contentHeight / 2
        duration: Kirigami.Units.longDuration * 2
        easing.type: Easing.OutCubic
        running: false
    }

    NumberAnimation on contentY {
        id: toClose
        from: contentY
        to: 0
        duration: Kirigami.Units.longDuration * 2
        easing.type: Easing.OutCubic
        running: false
    }

    // snap to end
    MouseArea {
        anchors.fill: footerLayout
        propagateComposedEvents: true
        onPressed: {
            toOpen.stop();
            toClose.stop();
            propagateComposedEvents = true;
        }
        onReleased: footerItem.resetToBoundsOnFlick()
    }

    onMovementStarted: {
        toOpen.stop();
        toClose.stop();
    }
    onFlickStarted: resetToBoundsOnFlick()
    onMovementEnded: resetToBoundsOnFlick()
    onHeightChanged: resetToBoundsOnResize()

    property var syncedLyricsModel: LyricsModel {
        id: syncedLyrics
    }

    property var videoInfoExtractor: VideoInfoExtractor {
        id: info

        videoId: UserPlaylistModel.currentVideoId
        onTitleChanged: {
            let index = UserPlaylistModel.index(UserPlaylistModel.currentIndex, 0)
            let videoId = UserPlaylistModel.data(index, UserPlaylistModel.VideoId)
            let title = UserPlaylistModel.data(index, UserPlaylistModel.Title)
            let artist = UserPlaylistModel.data(index, UserPlaylistModel.Artists)
            let album = UserPlaylistModel.data(index, UserPlaylistModel.Album)
            let duration = UserPlaylistModel.data(index, UserPlaylistModel.Duration)

            syncedLyrics.song = title
            syncedLyrics.artist = artist
            syncedLyrics.album = album
            syncedLyrics.duration = duration
            syncedLyrics.fetchLyrics()

            Library.addPlaybackHistoryItem(videoId, title, artist, album)
        }
    }

    ThumbnailSource {
        id: thumbnailSource
        videoId: UserPlaylistModel.currentVideoId
    }

    property MediaPlayer audioPlayer: audioLoader.item
    property AudioOutput audioOutput: audioLoader.item.audioOutput

    Loader {
        id: audioLoader
        active: true
        asynchronous: true
        sourceComponent: MediaPlayer {
            id: audio

            property int prevPlaybackState: MediaPlayer.StoppedState
            source: info.audioUrl
            onSourceChanged: play()

            onPlaybackStateChanged : {
                if (mediaStatus == MediaPlayer.BufferingMedia && playbackState == MediaPlayer.StoppedState && prevPlaybackState == MediaPlayer.PlayingState) {
                    Qt.callLater(function() {
                        play()
                        console.log("force play on stuck with BufferingMedia & StoppedState");
                    })
                }
                prevPlaybackState = playbackState
            }

            onMediaStatusChanged: {
                if (mediaStatus === MediaPlayer.EndOfMedia) {
                    console.log("Song ended");
                    UserPlaylistModel.next();
                }
            }

            audioOutput: AudioOutput {
                id: audioOutput
            }
        }
    }

    contentHeight: height * 2

    ColumnLayout {
        id: footerLayout
        y:-footerSpacing
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: 0

        // margin from top
        Item {
            Layout.minimumHeight: footerItem.height - footerItem.minimizedPlayerHeight
        }

        Controls.Control {
            Layout.fillWidth: true
            padding: 0
            implicitHeight: footerItem.minimizedPlayerHeight+footerSpacing

            // minimized player background
            background: Item {
                Rectangle {
                    anchors.fill: parent
                    color: Qt.rgba(25, 25, 30, 1)
                }

                Image {
                    opacity: 0.2
                    source: thumbnailSource.cachedPath
                    asynchronous: true

                    anchors.fill: parent
                    fillMode: Image.PreserveAspectCrop
                }

                layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software
                layer.effect: MultiEffect {
                    brightness: -0.7
                    saturation: 1

                    blur: 1.0
                    blurMultiplier: 3.0
                    blurEnabled: true
                    autoPaddingEnabled: false
                }

                Rectangle {
                    visible: GraphicsInfo.api === GraphicsInfo.Software
                    anchors.fill: parent
                    color: "black"
                    opacity: 0.8
                }
            }

            MinimizedPlayerControls {
                progressBarHeight: footerItem.progressBarHeight
                minimizedPlayerContentHeight: footerItem.minimizedPlayerContentHeight
                height: minimizedPlayerHeight

                focus: true
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top

                thumbnail: thumbnailSource.cachedPath
                info: footerItem.videoInfoExtractor
                audio: footerItem.audioPlayer

                onRequestOpen: toOpen.start();
            }
        }

        MaximizedPlayerPage {
            id: maximizedPlayerPage
            Layout.fillWidth: true
            Layout.fillHeight: true
            implicitHeight: footerItem.height

            thumbnail: footerItem.videoInfoExtractor.thumbnail
            info: footerItem.videoInfoExtractor
            audio: footerItem.audioPlayer
            syncedLyrics: footerItem.syncedLyricsModel

            onRequestClose: toClose.start();
        }
    }
}
