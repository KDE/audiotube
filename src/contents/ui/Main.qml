// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import QtMultimedia

import org.kde.audiotube
import org.kde.config as KConfig

Kirigami.ApplicationWindow {
    id: root
    minimumWidth: 300
    minimumHeight: 300
    pageStack.globalToolBar.style: wideScreen? Kirigami.ApplicationHeaderStyle.None: Kirigami.ApplicationHeaderStyle.Breadcrumb

    color: "transparent"
    Blur{id:blur}
    Component.onCompleted: {
        blur.setBlur(sidebar, true);

        switch (StateConfig.lastOpenedPage) {
        case 'libraryPage':
            pageStack.initialPage = pool.loadPage(Qt.resolvedUrl('./LibraryPage.qml'))
            break;
        case 'homePage':
            pageStack.initialPage = pool.loadPage(Qt.resolvedUrl('./HomePage.qml'))
            break;
        case 'favourites':
            pageStack.initialPage = pool.loadPage(Qt.resolvedUrl('./PlaybackHistory.qml#favourites'))
            break;
        case 'history':
            pageStack.initialPage = pool.loadPage(Qt.resolvedUrl('./PlaybackHistory.qml#history'))
            break;
        case 'explorePage':
            pageStack.initialPage = pool.loadPage(Qt.resolvedUrl('./ExplorePage.qml'))
            break;
        case 'playlists':
            pageStack.initialPage = pool.loadPage(Qt.resolvedUrl("./LocalPlaylistsPage.qml"))
            break;
        default:
            pageStack.initialPage = pool.loadPage(Qt.resolvedUrl('./HomePage.qml'))
        }
    }

    contentItem.parent.z: (playerFooter.contentY === 0) ? 0 : 2 // HACK: So contents can cover the header. This is only necessary since Qt 6.9.

    pageStack.columnView.columnResizeMode: Kirigami.ColumnView.SingleColumn

    property alias searchField: searchLoader // TODO
    property bool wideScreen: width >= 600
    property bool showSearch: false // only applicable if not widescreen

    KConfig.WindowStateSaver {
        configGroupName: "MainWindow"
    }

    // so that there is still a separator, since the header style is none
    Kirigami.Separator {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        visible: wideScreen
    }

    Kirigami.PagePool {
        id: pool
        onLastLoadedItemChanged: if (lastLoadedItem) {
            if (['homePage', 'libraryPage', 'favourites', 'explorePage', 'history', 'playlists'].includes(lastLoadedItem.objectName)) {
                StateConfig.lastOpenedPage = lastLoadedItem.objectName;
                StateConfig.save();
            }
        }
    }

    Loader {
        id: shareMenu
        active: false
        sourceComponent: ShareMenu {}
    }

    function openShareMenu(inputTitle: string, url: string) {
        shareMenu.active = true
        shareMenu.item.inputTitle = inputTitle
        shareMenu.item.url = url
        shareMenu.item.open()
    }

    Sidebar {
        id:sidebar
        visible: wideScreen
        height: pageStack.height - playerFooter.implicitHeight

    }
    NavigationBar{
        id: bottombar
        visible: !wideScreen
    }

    header: Controls.Control {
        visible: wideScreen || root.showSearch
        padding: Kirigami.Units.largeSpacing

        background: Rectangle {
             width: parent.width
             height: parent.height
             anchors.fill: parent
             Kirigami.Theme.inherit: false
             Kirigami.Theme.colorSet: Kirigami.Theme.Header
             color:  Kirigami.Theme.backgroundColor
         }

        contentItem: RowLayout {
            spacing: 0
            Row {
                Layout.alignment: Qt.AlignLeft

                Controls.ToolButton {
                    id: back
                    enabled: applicationWindow().pageStack.layers.depth > 1 || (applicationWindow().pageStack.depth > 1 && (applicationWindow().pageStack.currentIndex > 0 || applicationWindow().pageStack.contentItem.contentX > 0)) // Copied from https://invent.kde.org/frameworks/kirigami/-/blob/master/src/controls/templates/private/BackButton.qml#L16
                    icon.name: "draw-arrow-back"
                    onClicked: pageStack.goBack()
                    visible: wideScreen
                }
                Controls.ToolButton {
                    icon.name: "draw-arrow-forward"
                    enabled: applicationWindow().pageStack.currentIndex < applicationWindow().pageStack.depth-1
                    onClicked: pageStack.goForward()
                    visible: wideScreen
                }
            }

            // spacer
            Item {
                visible: wideScreen
                Layout.fillWidth: !root.wideScreen
            }
            SearchWithDropdown {
                id: searchLoader
                visible: root.wideScreen || root.showSearch
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Layout.fillHeight: true
                height: back.height
                width: wideScreen ? null : root.width
                Layout.maximumWidth: wideScreen ? 400 : root.width
            }

            Item {
                width: 2*back.width
                visible: wideScreen

            }
        }
    }

    title: i18n("AudioTube")

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }

    pageStack.clip: true

    function play(videoId) {
        UserPlaylistModel.initialVideoId = videoId
    }

    function playPlaylist(playlistId) {
        UserPlaylistModel.playlistId = playlistId
    }

    function playShufflePlaylist(playlistId) {
        UserPlaylistModel.shuffle = true
        UserPlaylistModel.playlistId = playlistId
    }

    function playFavourites(shuffle) {
        UserPlaylistModel.playFavourites(Library.favourites, shuffle)
    }

    function playPlaybackHistory(model, shuffle) {
        UserPlaylistModel.playPlaybackHistory(model, shuffle)
    }

    function focusSearch(){searchLoader.forceFocus()}

    Connections {
        target: ErrorHandler

        function onErrorOccurred(error) {
            showPassiveNotification(error)
        }
    }

    Component {
        id: searchAlbum

        ColumnLayout {
            id: mpdelegateItem

            required property string videoId
            required property string title

            width: 90
            Layout.maximumWidth: 70

            Kirigami.ShadowedRectangle {
                id: recCover
                MouseArea {
                    id: recArea
                    anchors.fill: parent
                    onClicked: play(mpdelegateItem.videoId)
                    hoverEnabled: !Kirigami.Settings.hasTransientTouchInput
                    onEntered: {
                        if (!Kirigami.Settings.hasTransientTouchInput) {
                            recSelected.visible = true
                            searchTitle.color = Kirigami.Theme.hoverColor
                            searchTitle.font.bold = true
                        }
                    }
                    onExited: {
                        recSelected.visible = false
                        searchTitle.color = Kirigami.Theme.textColor
                        searchTitle.font.bold = false
                    }
                }
                Layout.margins: 5

                width: 70
                height: 70
                radius: 10
                shadow.size: 15
                shadow.xOffset: 5
                shadow.yOffset: 5
                shadow.color: Qt.rgba(0, 0, 0, 0.2)
                Rectangle {
                    width: 70
                    height: 70

                    color: "transparent"

                    ThumbnailSource {
                        id: thumbnailSource
                        videoId: mpdelegateItem.videoId
                    }
                    RoundedImage {
                        source: thumbnailSource.cachedPath
                        height: parent.height
                        width: height
                        radius: 10
                    }
                    Rectangle {
                        id: recSelected

                        Rectangle {
                            anchors.fill: parent
                            color: Kirigami.Theme.hoverColor
                            radius: 10
                            opacity: 0.2
                        }

                        visible: false
                        anchors.fill: parent

                        radius: 9

                        border.color: Kirigami.Theme.hoverColor
                        border.width: 2
                        color: "transparent"
                    }
                }
            }
            Controls.Label {
                id: searchTitle
                leftPadding:5
                Layout.maximumWidth: 70
                text: mpdelegateItem.title
                elide: Qt.ElideRight
                wrapMode: Text.WordWrap
                Layout.maximumHeight: 40
            }
            Item {
                height: 5
            }
        }
    }

    pageStack.anchors.bottomMargin: wideScreen ? playerFooter.minimizedPlayerHeight: playerFooter.minimizedPlayerHeight+bottombar.height
    pageStack.anchors.leftMargin: wideScreen ? sidebar.width:0

    // media player
    PlayerFooter {
        id: playerFooter
        anchors.topMargin: -root.header.height
        anchors.fill: parent
        footerSpacing: wideScreen ? 0 : bottombar.height

        // only expand flicking area to full screen when it is open
        z: (contentY === 0) ? -1 : 999
    }

    MprisPlayer {
        id: mprisPlayer
        serviceName: "AudioTube"

        property string artist: playerFooter.videoInfoExtractor.artist
        property string songTitle: playerFooter.videoInfoExtractor.title ? playerFooter.videoInfoExtractor.title : i18n("No song playing")
        property int songLength: playerFooter.audioPlayer.duration * 1000
        property alias thumbnail: playerFooter.thumbnail

        function next() {
            if(UserPlaylistModel.canSkip) {
                UserPlaylistModel.next()
            }
            else{
                playerFooter.audioPlayer.stop()
            }
        }

        //Mpris2 Root interface
        identity: root.title
        supportedUriSchemes: []
        supportedMimeTypes: []
        desktopEntry: "org.kde.audiotube"

        //Mpris2 Player Interface
        canControl: true
        canGoNext: UserPlaylistModel.canSkip
        canGoPrevious: UserPlaylistModel.canSkipBack
        canPause: playerFooter.audioPlayer.status !== MediaPlayer.NoMedia
        canPlay: playerFooter.audioPlayer.status !== MediaPlayer.NoMedia
        canSeek: false

        playbackStatus: playerFooter.audioPlayer.playbackState === MediaPlayer.PlayingState ? Mpris.Playing : (playerFooter.audioPlayer.playbackState == MediaPlayer.PausedState ? Mpris.Paused : Mpris.Stopped)
        shuffle: false
        volume: playerFooter.audioOutput.muted ? 0.0 : playerFooter.audioOutput.volume
        position: playerFooter.audioPlayer.position * 1000

        onPauseRequested: playerFooter.audioPlayer.pause()
        onPlayRequested: playerFooter.audioPlayer.play()
        onPlayPauseRequested: {
            if(playerFooter.audioPlayer.playbackStatus === MediaPlayer.PlayingState) {
                playerFooter.audioPlayer.pause()
            }
            else if(playerFooter.PlayingState === MediaPlayer.PausedState) {
                PlayerFooter.audioPlayer.play()
            }
        }
        onStopRequested: playerFooter.audioPlayer.stop()
        onNextRequested: {
            next()
        }
        onPreviousRequested: {
            if(UserPlaylistModel.canSkipBack) {
                UserPlaylistModel.previous()
            }
            else{
                playerFooter.audioPlayer.stop()
            }
        }
        onSeekRequested: {
            if(canSeek) {
                if(playerFooter.audioPlayer.position + offset/1000 < 0) {
                    playerFooter.audioPlayer.seek(0)
                }
                else if(playerFooter.audioPlayer.position + offset/1000 > playerFooter.audioPlayer.duration) {
                    next()
                }
                else {
                    playerFooter.audioPlayer.seek(Math.floor(playerFooter.audioPlayer.position + offset/1000));
                }
                emitSeeked()
            }
        }
        onSetPositionRequested: {
            if(canSeek) {
                if(position >= 0 && position/1000 <= playerFooter.audioPlayer.duration) {
                    playerFooter.audioPlayer.seek(Math.floor(position/1000))
                    emitSeeked()
                }
            }
        }
        onShuffleRequested: {
            UserPlaylistModel.shufflePlaylist()
        }

        onArtistChanged: {
            var metadata = mprisPlayer.metadata
            metadata[Mpris.metadataToString(Mpris.Artist)] = [artist]
            mprisPlayer.metadata = metadata
        }
        onSongTitleChanged: {
            var metadata = mprisPlayer.metadata
            metadata[Mpris.metadataToString(Mpris.Title)] = songTitle
            mprisPlayer.metadata = metadata
        }
        onSongLengthChanged: {
            var metadata = mprisPlayer.metadata
            metadata[Mpris.metadataToString(Mpris.Length)] = songLength
            mprisPlayer.metadata = metadata
        }
        onThumbnailChanged: {
            var metadata = mprisPlayer.metadata
            metadata[Mpris.metadataToString(Mpris.ArtUrl)] = thumbnail
            mprisPlayer.metadata = metadata
        }
    }
}
