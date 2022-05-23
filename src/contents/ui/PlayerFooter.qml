// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
// SPDX-FileCopyrightText: 2021 Bart De Vries <bart@mogwai.be>
// SPDX-FileCopyrightText: 2020-2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12 as Controls

import QtMultimedia 5.12
import QtGraphicalEffects 1.0

import org.kde.kirigami 2.12 as Kirigami
import org.kde.ytmusic 1.0

Flickable {
    id: footerItem
    
    property bool maximized: false
    
    readonly property int progressBarHeight: Kirigami.Units.gridUnit / 6
    readonly property int minimizedPlayerContentHeight: Math.round(Kirigami.Units.gridUnit * 3.5)
    readonly property int minimizedPlayerHeight: minimizedPlayerContentHeight + progressBarHeight

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

    property var videoInfoExtractor: VideoInfoExtractor {
        id: info

        onSongChanged: audio.play()
        videoId: UserPlaylistModel.currentVideoId
        onTitleChanged: Library.addPlaybackHistoryItem(info.videoId, info.title)
    }
    
    property var audioPlayer: Audio {
        id: audio

        source: info.audioUrl
        onStatusChanged: {
            if (status === Audio.EndOfMedia) {
                console.log("Song ended");
                UserPlaylistModel.next();
            }
        }
    }
    
    contentHeight: height * 2
    
    ColumnLayout {
        id: footerLayout
        
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
            implicitHeight: footerItem.minimizedPlayerHeight
            
            // minimized player background
            background: Item {
                Rectangle {
                    anchors.fill: parent
                    color: Qt.rgba(25, 25, 30, 1)
                }
                
                Image {
                    opacity: 0.2
                    source: info.thumbnail
                    asynchronous: true
                    
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectCrop
                }
                
                layer.enabled: true
                layer.effect: HueSaturation {
                    cached: true

                    lightness: -0.5
                    saturation: 1.9

                    layer.enabled: true
                    layer.effect: FastBlur {
                        cached: true
                        radius: 64
                        transparentBorder: false
                    }
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
            
            info: footerItem.videoInfoExtractor
            audio: footerItem.audioPlayer
                
            onRequestClose: toClose.start();
        }
        
        //GridLayout {
            //id: playerLayout

            //readonly property bool mobile: width > height

            //Layout.fillHeight: footerItem.maximized
            //Layout.fillWidth: true
            //flow: width > height ? GridLayout.LeftToRight : GridLayout.TopToBottom
            //visible: footerItem.maximized

            //ColumnLayout {
                //Layout.preferredWidth: footerLayout.width * (playerLayout.mobile ? 1 : 0.5)
                //Layout.preferredHeight: footerLayout.height * (playerLayout.mobile ? 0.5 : 1)

                //Item {
                    //Layout.fillWidth: true
                    //Layout.fillHeight: true

                    //Image {
                        //anchors.fill: parent
                        //source: info.thumbnail
                        //fillMode: Image.PreserveAspectFit
                    //}
                //}
                //Controls.RoundButton {
                    //readonly property QtObject favouriteWatcher: Library.favouriteWatcher(info.videoId)

                    //Layout.alignment: Qt.AlignHCenter
                    //icon.name: favouriteWatcher ? (favouriteWatcher.isFavourite ? "starred-symbolic" : "non-starred-symbolic") : ""
                    //onClicked: if (favouriteWatcher)
                                   //favouriteWatcher.isFavourite
                                    //? Library.removeFavourite(info.videoId)
                                    //: Library.addFavourite(info.videoId, info.title)
                //}
            //}

            //Item {
                //Layout.fillWidth: true
                //Layout.fillHeight: true
                //Layout.preferredWidth: footerLayout.width * (playerLayout.mobile ? 1 : 0.5)
                //Layout.preferredHeight: footerLayout.height * (playerLayout.mobile ? 0.5 : 1)

                //Controls.ScrollView {
                    //anchors.fill: parent

                    //ListView {
                        //anchors.fill: parent
                        //id: playlistView

                        //clip: true

                        //Controls.BusyIndicator {
                            //anchors.centerIn: parent
                            //visible: UserPlaylistModel.loading || UserPlaylistModel.loading
                        //}

                        //onCountChanged: {
                            //if (count < 1) {
                                //footerItem.maximized = false
                            //}
                        //}

                        //model: UserPlaylistModel

                        //delegate: Kirigami.SwipeListItem {
                            //id: delegateItem
                            //required property string title
                            //required property string videoId
                            //required property string artists
                            //required property bool isCurrent

                            //highlighted: isCurrent
                            //onClicked: UserPlaylistModel.skipTo(videoId)

                            //ColumnLayout {
                                //Layout.fillWidth: true
                                //Kirigami.Heading {
                                    //elide: Text.ElideRight
                                    //Layout.fillWidth: true
                                    //level: 2
                                    //text: title
                                //}

                                //Controls.Label {
                                    //elide: Text.ElideRight
                                    //Layout.fillWidth: true
                                    //text: artists
                                //}
                            //}

                            //actions: [
                                //Kirigami.Action {
                                    //text: i18n("Remove Track")
                                    //icon.name: "list-remove"
                                    //onTriggered: UserPlaylistModel.remove(delegateItem.videoId)
                                //}
                            //]
                        //}

                        //header: Controls.ToolBar {
                            //anchors.left: parent.left
                            //anchors.right: parent.right
                            //RowLayout {
                                //anchors.fill: parent
                                //Item {
                                    //Layout.fillWidth: true
                                //}

                                //Controls.ToolButton {
                                    //text: i18n("Clear")
                                    //icon.name: "edit-clear-all"
                                    //onClicked: UserPlaylistModel.clear()
                                //}
                                //Controls.ToolButton {
                                    //text: i18n("Shuffle")
                                    //icon.name: "media-playlist-shuffle"
                                    //onClicked: UserPlaylistModel.shufflePlaylist()
                                //}
                            //}
                        //}
                    //}
                //}
            //}
        //}

        //Kirigami.Separator {
            //Layout.fillWidth: true
        //}

        //RowLayout {
            //id: player

            //Layout.fillWidth: true
            //Layout.fillHeight: !footer.maximized

            //Controls.ToolButton {
                //display: Controls.AbstractButton.IconOnly
                //Layout.preferredWidth: parent.height
                //Layout.fillHeight: true
                //enabled: info.audioUrl != ""
                //text: audio.playbackState === Audio.PlayingState ? i18n("Pause") : i18n("Play")
                //visible: !info.loading
                //icon.name: audio.playbackState === Audio.PlayingState ? "media-playback-pause" : "media-playback-start"
                //onClicked: audio.playbackState === Audio.PlayingState ? audio.pause() : audio.play()
            //}

            //Controls.ToolButton {
                //display: Controls.AbstractButton.IconOnly
                //Layout.preferredWidth: parent.height
                //Layout.fillHeight: true
                //enabled: UserPlaylistModel.canSkip
                //visible: !info.loading
                //icon.name: "media-skip-forward"
                //onClicked: UserPlaylistModel.next()
            //}

            //Controls.BusyIndicator {
                //Layout.fillHeight: true
                //visible: info.loading
            //}

            //ColumnLayout {
                //Layout.fillHeight: true
                //Layout.topMargin: Kirigami.Units.smallSpacing
                //Layout.bottomMargin: Kirigami.Units.smallSpacing

                //Kirigami.Heading {
                    //level: 3
                    //Layout.fillWidth: true
                    //elide: Qt.ElideRight
                    //text: info.title ? info.title : i18n("No media playing")
                //}

                //RowLayout{

                    //Controls.Label{
                        //visible: info.title
                        //text: PlayerUtils.formatTimestamp(audio.position)
                    //}

                    //Controls.Slider {
                        //Layout.fillWidth: true
                        //from: 0
                        //to: audio.duration
                        //value: audio.position
                        //enabled: audio.seekable
                        //onMoved: {
                            //console.log("Value:", value);
                            //audio.seek(Math.floor(value));
                        //}

                        //Behavior on value {
                            //NumberAnimation {
                                //duration: 1000
                            //}

                        //}

                    //}

                    //Controls.Label{
                        //visible: info.title
                        //text: PlayerUtils.formatTimestamp(audio.duration)
                    //}

                //}



            //}

            //Controls.ToolButton {
                //display: Controls.AbstractButton.IconOnly
                //Layout.preferredWidth: parent.height
                //text: i18n("Expand")
                //enabled: playlistView.count > 0
                //Layout.fillHeight: true
                //icon.name: footerItem.maximized ? "arrow-down" : "arrow-up"
                //onClicked: footerItem.maximized = !footerItem.maximized
            //}

        //}
    }

    //Behavior on height {
        //NumberAnimation {
        //}
    //}
}
