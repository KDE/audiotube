// SPDX-FileCopyrightText: 2020-2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import QtGraphicalEffects 1.0
import QtMultimedia 5.12

import org.kde.kirigami 2.19 as Kirigami
import org.kde.ytmusic 1.0

Item {
    id: root

    required property var info // VideoInfoExtractor object
    required property var audio // Audio object
    readonly property bool isWidescreen: width >= Kirigami.Units.gridUnit * 50

    signal requestClose()

    // background image


    Item {
        id:bg
        anchors.fill: parent

        Rectangle {
            anchors.fill: parent
            color: Qt.rgba(25, 25, 30, 1)
        }

        Image {
            scale: 1.8
            anchors.fill: parent
            asynchronous: true

            source: info.thumbnail
            fillMode: Image.PreserveAspectCrop

            sourceSize.width: 512
            sourceSize.height: 512
        }

        layer.enabled: true
        layer.effect: HueSaturation {
            cached: true

            lightness: -0.5
            saturation: 1.9

            layer.enabled: true
            layer.effect: FastBlur {
                cached: true
                radius: 100
            }


        }

    }



    Rectangle {
        anchors.fill: parent
        gradient: Gradient{
            GradientStop { position: 0.0; color: "transparent" }
            GradientStop { position: 1.1; color: "black"  }
        }
    }

        // content
    RowLayout{
        anchors.fill: parent
        
        ColumnLayout {
            Layout.fillWidth: true
            // hide arrow button
            ToolButton {
                id: closeButton
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.maximumHeight: parent.height
                Layout.preferredHeight: Kirigami.Units.gridUnit * 3
                Layout.maximumWidth: parent.height
                Layout.preferredWidth: Kirigami.Units.gridUnit * 3
                Layout.topMargin: Kirigami.Units.smallSpacing
                icon.name: "arrow-down"
                icon.color: "white"
                Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                Kirigami.Theme.inherit: false
                onClicked: root.requestClose()
            }

            SwipeView {
                interactive: false
                Layout.fillHeight: true
                Layout.fillWidth: true
                clip:true
                id: swipeView
                property double specWidth: {
                    let allowedWidth = root.width - Kirigami.Units.largeSpacing * 4;
                    let allowedHeight = root.height - Kirigami.Units.largeSpacing * 16 - (closeButton.height + bottomPlayerControls.height);
                    if (allowedWidth > allowedHeight) {
                        return allowedHeight;
                    } else {
                        return allowedWidth;
                    }
                }
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.preferredHeight: specWidth

                // music art
                Flickable {
                    flickableDirection: Flickable.HorizontalFlick
                    clip:true
                    contentWidth: coverArt.width
                    contentHeight: coverArt.height
                    height: swipeView.height
                    width: applicationWindow().width-sideDrawer.width
                    onFlickEnded:{

                        if(horizontalVelocity<0){
                            UserPlaylistModel.next()
                        }
                        else{
                            if(UserPlaylistModel.canSkipBack){
                                UserPlaylistModel.previous()
                            }
                        }
                    }
                    Item{
                        height:swipeView.height
                        width:applicationWindow().width-sideDrawer.width
                        Kirigami.ShadowedRectangle {
                           id:coverArt
                           x: 4
                           anchors.centerIn: parent
                           width: swipeView.specWidth
                           height: swipeView.specWidth


                            color: "transparent"
                            radius: 10
                            shadow.size: 15
                            shadow.xOffset: 5
                            shadow.yOffset: 5
                            shadow.color: Qt.rgba(0, 0, 0, 0.2)
                            RoundedImage {
                                source: info.thumbnail
                                height: parent.height
                                width: height
                                radius: 10
                            }
                        }
                }
                }

                ColumnLayout{
                    width: swipeView.width
                    height: swipeView.height
                    ScrollView {
                        Layout.maximumWidth: 900
                        contentWidth: -1
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.alignment: Qt.AlignHCenter
                        clip: true

                        //contentY: audio.position / audio.duration

                        Label {
                            padding: 20
                            text: UserPlaylistModel.lyrics
                            color: "white"
                        }
                    }
                }
            }

            ColumnLayout {
                id: bottomPlayerControls
                Layout.topMargin: Kirigami.Units.largeSpacing
                Layout.leftMargin: Kirigami.Units.gridUnit * 2
                Layout.rightMargin: Kirigami.Units.gridUnit * 2
                Layout.bottomMargin: Kirigami.Units.gridUnit * 0.5

                // song name
                Label {
                    id: mainLabel
                    text: info.title ? info.title : i18n("No media playing")

                    Layout.fillWidth: true

                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                    maximumLineCount: 1
                    // Hardcoded because the footerbar blur always makes a dark-ish
                    // background, so we don't want to use a color scheme color that
                    // might also be dark
                    color: "white"
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.5
                    font.weight: Font.Bold
                    font.bold: true
                }

                // song artist
                Kirigami.Heading {
                    id: authorLabel
                    text: info.artist ? info.artist : info.channel
                    color: Kirigami.Theme.disabledTextColor

                    Layout.fillWidth: true
                    Layout.maximumWidth:600
                    Layout.alignment: Qt.AlignHCenter
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                    maximumLineCount: 1
                    // Hardcoded because the footerbar blur always makes a dark-ish
                    // background, so we don't want to use a color scheme color that
                    // might also be dark
                    opacity: 0.9
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.3
                    font.bold: true
                    Layout.bottomMargin: Kirigami.Units.gridUnit
                }
                RowLayout {
                    Layout.topMargin: Kirigami.Units.gridUnit

                    id: controlButtonBox
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillHeight: true
                    spacing: 2


                    Button {
                        id: skipBackwardButton
                        implicitHeight: 40
                        implicitWidth: 40

                        Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                        Kirigami.Theme.inherit: false

                        enabled: UserPlaylistModel.canSkipBack
                        onClicked: UserPlaylistModel.previous()
                        contentItem: Item{
                            Kirigami.Icon {
                                anchors.centerIn:parent
                                source:"media-skip-backward"
                                color: "white"
                                width: Kirigami.Units.gridUnit
                                height: Kirigami.Units.gridUnit

                            }
                        }
                        background: Kirigami.ShadowedRectangle{
                            corners.topLeftRadius: 7
                            corners.bottomLeftRadius: 7


                            color: if (parent.down){
                                    Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.3)
                                }else if(parent.hovered){
                                    Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.7)
                                }else{
                                    Qt.rgba(1, 1, 1, 0.2)
                                }
                        }
                    }

                    Button {
                        id: playPauseButton
                        implicitHeight: 40
                        implicitWidth: 60
                        onClicked: audio.playbackState === Audio.PlayingState ? audio.pause() : audio.play()
                        contentItem: Item{
                            Kirigami.Icon {
                                anchors.centerIn:parent
                                source: audio.playbackState === Audio.PlayingState ? "media-playback-pause" : "media-playback-start"
                                color: "white"
                                width: Kirigami.Units.gridUnit
                                height: Kirigami.Units.gridUnit
                            }
                        }
                        background: Kirigami.ShadowedRectangle{
                            color: if (parent.down){
                                    Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.3)
                                }else if(parent.hovered){
                                    Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.7)
                                }else{
                                    Qt.rgba(1, 1, 1, 0.2)
                                }
                        }
                    }

                    Button {
                        id: skipForwardButton
                        implicitHeight: 40
                        implicitWidth: 40
                        Layout.rightMargin:isWidescreen?0:10

                        Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                        Kirigami.Theme.inherit: false

                        enabled: UserPlaylistModel.canSkip
                        onClicked: UserPlaylistModel.next()
                        contentItem: Item{
                            Kirigami.Icon {
                                anchors.centerIn:parent
                                source:"media-skip-forward"
                                color: "white"
                                width: Kirigami.Units.gridUnit
                                height: Kirigami.Units.gridUnit

                            }
                        }
                        background: Kirigami.ShadowedRectangle{
                            corners.topRightRadius: 7
                            corners.bottomRightRadius: 7
                            color: if (parent.down){
                                    Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.3)
                                }else if(parent.hovered){
                                    Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.7)
                                }else{
                                    Qt.rgba(1, 1, 1, 0.2)
                                }
                        }
                    }
                }
                // slider row
                RowLayout {
                    Layout.topMargin: Kirigami.Units.gridUnit
                    spacing: Kirigami.Units.smallSpacing

                    Label {
                        Layout.alignment: Qt.AlignVCenter
                        color: "white"
                        visible: info.title
                        text: PlayerUtils.formatTimestamp(audio.position)
                    }

                    Slider {
                        Layout.alignment: Qt.AlignVCenter
                        Layout.fillWidth: true
                        from: 0
                        to: audio.duration
                        value: audio.position
                        enabled: audio.seekable
                        onMoved: {
                            console.log("Value:", value);
                            audio.seek(Math.floor(value));
                        }

                        Behavior on value {
                            NumberAnimation {
                                duration: 1000
                            }
                        }
                    }

                    Label {
                        Layout.alignment: Qt.AlignVCenter
                        color: "white"
                        visible: info.title
                        text: PlayerUtils.formatTimestamp(audio.duration)
                    }
                }

                RowLayout {
                    Layout.topMargin: Kirigami.Units.largeSpacing
                    Layout.fillWidth: true
                    // ensure white icons
                    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                    Kirigami.Theme.inherit: false
                    Item {
                        width: queueButton.width
                        visible: wideScreen
                    }

                    Item { Layout.fillWidth: true}

                    ToolButton {
                        id: favouriteButton
                        readonly property QtObject favouriteWatcher: Library.favouriteWatcher(info.videoId)
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        Layout.preferredHeight: Kirigami.Units.gridUnit * 2.5
                        Layout.maximumWidth: height
                        Layout.preferredWidth: height
                        onClicked: {
                            if (favouriteWatcher) {
                                if (favouriteWatcher.isFavourite) {
                                    Library.removeFavourite(info.videoId)
                                    // This would insert slightly ugly data into the database, but let's hope the song is already saved
                                } else {
                                    let index = UserPlaylistModel.index(UserPlaylistModel.currentIndex, 0)
                                    let videoId = UserPlaylistModel.data(index, UserPlaylistModel.VideoId)
                                    let title = UserPlaylistModel.data(index, UserPlaylistModel.Title)
                                    let artist = UserPlaylistModel.data(index, UserPlaylistModel.Artists)
                                    let album = UserPlaylistModel.data(index, UserPlaylistModel.Album)
                                    Library.addFavourite(videoId, title, artist, album)
                                }
                            }
                        }
                        text: "Favourite"
                        icon.name: favouriteWatcher ? (favouriteWatcher.isFavourite ? "starred-symbolic" : "non-starred-symbolic") : ""
                        icon.color: "white"
                        display: AbstractButton.IconOnly
                        Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                        Kirigami.Theme.inherit: false
                    }

                    ToolButton {
                        id: volumeButtonSmallScreen
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.preferredHeight: Kirigami.Units.gridUnit * 2.5
                        Layout.maximumWidth: height
                        Layout.preferredWidth: height
                        visible: !isWidescreen
                        enabled: !isWidescreen

                        icon.name: muteButton.icon.name

                        onClicked:{
                            if(!volumeDrawer.opened){
                                volumeDrawer.open()
                                volumeDrawer.interactive = true
                            }
                            else{
                                volumeDrawer.close()
                            }
                        }
                        Drawer {
                            id: volumeDrawer

                            edge: Qt.BottomEdge
                            width: applicationWindow().width
                            height: contents.height + 40
                            interactive: false
                            background: Kirigami.ShadowedRectangle{
                                corners.topRightRadius: 10
                                corners.topLeftRadius: 10
                                shadow.size: 20
                                shadow.color: Qt.rgba(0, 0, 0, 0.5)
                                color: Kirigami.Theme.backgroundColor
                            }
                            onClosed: {
                                interactive = false
                            }
                            ColumnLayout {
                                width: volumeDrawer.width

                                Rectangle {
                                    Layout.margins: 5
                                    radius:50
                                    Layout.alignment: Qt.AlignHCenter
                                    color: Kirigami.Theme.textColor
                                    opacity: 0.7
                                    width: 40
                                    height: 4

                                }
                                RowLayout {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    id: contents
                                    ToolButton{
                                        Layout.preferredHeight: Kirigami.Units.gridUnit * 2.5
                                        Layout.maximumWidth: height
                                        Layout.preferredWidth: height

                                        icon.name: muteButton.icon.name
                                        checkable: true
                                        checked: muteButton.checked
                                        onClicked: {
                                            if(audio.muted)
                                            {
                                                muteButton.unmuteAudio()
                                            }
                                            else
                                            {
                                                muteButton.muteAudio()
                                            }
                                        }
                                    }

                                    Slider {
                                        id: slider
                                        value: volumeSlider.value
                                        opacity: volumeSlider.opacity
                                        wheelEnabled: true

                                        Layout.fillWidth: true
                                        Layout.preferredHeight: Kirigami.Units.gridUnit * 2.5

                                        onMoved: {
                                            volumeSlider.value = value
                                            volumeSlider.valueChanged()
                                        }
                                    }

                                    Label {
                                        Layout.preferredHeight: slider.height
                                        Layout.preferredWidth: Kirigami.Units.gridUnit * 2.5

                                        text: volumeLabel.text
                                    }
                                }
                            }
                        }
                    }

                    ToolButton {
                        id: muteButton

                        function muteAudio() {
                            audio.muted = true
                            volumeSlider.opacity = 0.5
                            checked = true
                        }
                        function unmuteAudio() {
                            audio.muted = false
                            volumeSlider.opacity = 1
                            checked = false
                        }

                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.preferredHeight: Kirigami.Units.gridUnit * 2.5
                        Layout.maximumWidth: height
                        Layout.preferredWidth: height

                        onClicked: {
                            if(audio.muted) {
                                unmuteAudio()
                            }
                            else {
                                muteAudio()
                            }
                        }

                        icon.name: audio.muted ? "audio-volume-muted" : (volumeSlider.value < .33 ? "audio-volume-low" : (volumeSlider.value < .66 ? "audio-volume-medium" : "audio-volume-high"))
                        icon.color: "white"
                        
                        Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                        Kirigami.Theme.inherit: false
                        
                        checkable: true
                        visible: isWidescreen
                        enabled: isWidescreen
                    }

                    Slider {
                        id: volumeSlider
                        enabled: isWidescreen
                        visible: isWidescreen

                        property real volume: QtMultimedia.convertVolume(value, QtMultimedia.LogarithmicVolumeScale, QtMultimedia.LinearVolumeScale)

                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.preferredHeight: Kirigami.Units.gridUnit * 2.5
                        Layout.preferredWidth: 2*Layout.preferredHeight

                        value: 1.0
                        from: 0.0
                        to: 1.0
                        wheelEnabled: true

                        onMoved: {
                            valueChanged()
                        }
                        function valueChanged() {
                            audio.volume = volume
                            if(value==0) {
                                muteButton.muteAudio()
                            }
                            else{
                                muteButton.unmuteAudio()
                            }
                        }
                    }

                    Label {
                        id: volumeLabel

                        enabled: isWidescreen
                        visible: isWidescreen
                        text: i18n("%1%", Math.round(volumeSlider.value*100))

                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.preferredHeight: volumeSlider.Layout.preferredHeight
                        Layout.preferredWidth: Kirigami.Units.gridUnit * 2.5
                    }

                    ToolButton {
                        property bool lyricsShown: false
                        id: lyricsButton
                        Layout.preferredHeight: Kirigami.Units.gridUnit * 2.5
                        Layout.maximumWidth: height
                        Layout.preferredWidth: height
                        checked: lyricsShown
                        onClicked: {
                                if (!lyricsShown)
                                    swipeView.setCurrentIndex(1)
                                else{
                                    swipeView.setCurrentIndex(0)
                                }
                                lyricsShown = !lyricsShown
                        }
                        text: "Lyrics"
                        icon.name: "view-media-lyrics"
                        icon.color: "white"
                        display: AbstractButton.IconOnly

                        Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                        Kirigami.Theme.inherit: false
                    }

                    Item {
                        Layout.fillWidth: true
                        visible: wideScreen
                    }

                    ToolButton {
                        id: queueButton
                        Layout.alignment: Qt.AlignRight
                        Layout.preferredHeight: Kirigami.Units.gridUnit * 2.5
                        Layout.maximumWidth: height
                        Layout.preferredWidth: height
                        checked: !sideDrawer.collapsed && wideScreen

                        onClicked: {
                            if (wideScreen){
                                if (!sideDrawer.collapsed)
                                    collapse.running = true
                                else{
                                    sideDrawer.visible=true
                                    show.running = true
                                }
                                sideDrawer.collapsed = !sideDrawer.collapsed
                            }else{queueDrawer.open()}
                        }

                        text: "Queue"
                        display: AbstractButton.IconOnly
                        icon.name: "amarok_playlist"
                        icon.color: "white"

                        Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                        Kirigami.Theme.inherit: false
                    }

                    Item {
                        Layout.fillWidth: true
                        visible: !wideScreen
                    }

                }
            }
        }

        Item {
            onWidthChanged: if(!wideScreen) {collapse.running=true; collapsed=true}
            property bool collapsed: true
            id: sideDrawer
            Layout.fillWidth: true
            Layout.maximumWidth: 0
            Layout.preferredWidth: 350

            Layout.fillHeight: true
            visible: false
            NumberAnimation on Layout.maximumWidth {
                id: collapse
                easing.type: Easing.OutCubic
                running: false
                from: sideDrawer.Layout.preferredWidth; to: 0
                onFinished: { sideDrawer.visible=false}
            }
            NumberAnimation on Layout.maximumWidth {
                id: show
                easing.type: Easing.OutCubic
                running: false
                from: 0; to: 350
                //onFinished: { sideDrawer.visible=false}
            }
            Kirigami.Separator{
                color: "white"
                opacity: 0.3
                height: parent.height
                anchors.left: parent.left

            }
            Rectangle{
                anchors.fill: parent
                color: "white"
                opacity: 0.2
            }
            ColumnLayout {
                spacing: 0  
                anchors.fill: parent
                ScrollView {
                    id: playListScrollView

                    contentWidth: availableWidth

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ListView {
                        id: playListView
                        spacing: 5
                        rightMargin: 10
                        leftMargin:10
                        topMargin:10
                        bottomMargin: 10
                        clip: true
                        contentWidth: playListScrollView.contentWidth - rightMargin - leftMargin

                        Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                        Kirigami.Theme.inherit: false

                        BusyIndicator {
                            anchors.centerIn: parent
                            visible: UserPlaylistModel.loading || UserPlaylistModel.loading
                        }

                        model: UserPlaylistModel

                        delegate: Kirigami.SwipeListItem {
                            id: delegateItem
                            required property string title
                            required property string videoId
                            required property string artists
                            required property bool isCurrent
                            required property int index
                            
                            width: playListView.contentWidth
                            alwaysVisibleActions: true

                            background: Rectangle{
                                radius: 7
                                color:
                                    if (parent.down){
                                        Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.3)
                                    }else if(parent.hovered){
                                        Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.7)
                                    }else if(parent.highlighted){
                                        Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.7)
                                    }else{
                                        Qt.rgba(0, 0, 0, 0.4)
                                    }

                                border.color:
                                                if (parent.down){
                                                    Kirigami.Theme.hoverColor
                                                }else if(parent.hovered){
                                                    Kirigami.Theme.hoverColor
                                                }else{
                                                    Qt.rgba(1, 1, 1, 0)
                                                }

                                border.width: 1
                            }
                            highlighted: isCurrent
                            onClicked: UserPlaylistModel.skipTo(videoId)
                            contentItem: RowLayout{
                                ThumbnailSource {
                                    id: delegateThumbnailSource
                                    videoId: delegateItem.videoId
                                }
                                RoundedImage {
                                    source: delegateThumbnailSource.cachedPath
                                    Layout.margins: 2.5
                                    height: delegateItem.height
                                    width: height
                                    radius: 5
                                }

                                ColumnLayout {
                                    Layout.margins: 5

                                    Layout.fillWidth: true
                                    Kirigami.Heading {
    //                                        Layout.leftMargin: 5
    //                                        Layout.rightMargin: 5
    //                                        Layout.topMargin: 5

                                        elide: Text.ElideRight
                                        Layout.fillWidth: true
                                        level: 2
                                        text: title
                                    }

                                    Label {

                                        elide: Text.ElideRight
                                        Layout.fillWidth: true
                                        color: Kirigami.Theme.disabledTextColor
                                        text: artists
                                    }
                                }
                            }
                            actions: [
                                Kirigami.Action {
                                    text: i18n("Remove Track")
                                    icon.name: "list-remove"
                                    icon.color: "white"
                                    onTriggered: UserPlaylistModel.remove(delegateItem.videoId)
                                }
                            ]
                        }
                    }
                }
                
                Kirigami.Separator {
                    color: "white"
                    opacity: 0.3
                    Layout.fillWidth: true
                }
                
                RowLayout{
                    Layout.margins: Kirigami.Units.gridUnit * 0.5
                    Item {
                        Layout.fillWidth: true
                    }
                    
                    ToolButton {
                        id: clearPlaylistButton

                        Layout.preferredHeight: Kirigami.Units.gridUnit * 2.5
                        Layout.maximumWidth: height
                        Layout.preferredWidth: height
                        
                        text: i18n("Clear Playlist")
                        icon {
                            name: "edit-clear-all"
                            color: "white"
                        }
                        display: AbstractButton.IconOnly
                        
                        onClicked: UserPlaylistModel.clearExceptCurrent()
                        
                        Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                        Kirigami.Theme.inherit: false
                    }
                    
                    ToolButton {
                        id: shuffleButton
                        Layout.preferredHeight: Math.round(Kirigami.Units.gridUnit * 2.5)
                        Layout.maximumWidth: height
                        Layout.preferredWidth: height
                        
                        onClicked: UserPlaylistModel.shufflePlaylist()
                        
                        text: i18n("Shuffle")

                        icon {
                            name: "media-playlist-shuffle"
                            color: "white"
                        }
                        display: AbstractButton.IconOnly
                        Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                        Kirigami.Theme.inherit: false
                    }
                    
                    Item {
                        Layout.fillWidth: true
                    }
                }
            }
        }

        Drawer {
            id: queueDrawer
            onClosed:{sideDrawer.collapsed=true}
            edge: Qt.BottomEdge
            width: applicationWindow().width
            height: applicationWindow().height-50
//            interactive: false
            background: Kirigami.ShadowedRectangle{
                corners.topRightRadius: 10
                corners.topLeftRadius: 10
                shadow.size: 20
                shadow.color: Qt.rgba(0, 0, 0, 0.5)
                color: Kirigami.Theme.backgroundColor
            }

            contentItem: ColumnLayout{
                spacing:0
                Rectangle {
                    Layout.margins: 5
                    radius:50
                    Layout.alignment: Qt.AlignHCenter
                    color: Kirigami.Theme.textColor
                    opacity: 0.7
                    width: 40
                    height: 4
                }
                
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ListView {
                        topMargin: 10
                        clip: true

                        BusyIndicator {
                            anchors.centerIn: parent
                            visible: UserPlaylistModel.loading || UserPlaylistModel.loading
                        }

                        model: UserPlaylistModel

                        delegate: Kirigami.SwipeListItem {

                            id: drawerDelegateItem
                            required property string title
                            required property string videoId
                            required property string artists
                            required property bool isCurrent
                            required property int index

                            backgroundColor:"transparent"
                            highlighted: isCurrent
                            onClicked: {
                                queueDrawer.close()
                                UserPlaylistModel.skipTo(videoId)
                            }
                            RowLayout{
                                Layout.fillWidth: true
                                ThumbnailSource {
                                    id: drawerDelegateThumbnailSource
                                    videoId: drawerDelegateItem.videoId
                                }
                                RoundedImage {
                                    source: drawerDelegateThumbnailSource.cachedPath
                                    height: 50
                                    width: height
                                    radius: 5
                                }

                                ColumnLayout {
                                    Layout.margins: 5
                                    Layout.fillWidth: true
                                    Kirigami.Heading {
                                        elide: Text.ElideRight
                                        Layout.fillWidth: true
                                        level: 2
                                        text: title
                                    }

                                    Label {
                                        elide: Text.ElideRight
                                        Layout.fillWidth: true
                                        color: Kirigami.Theme.disabledTextColor
                                        text: artists
                                    }
                                }
                            }
                            actions: [
                                Kirigami.Action {
                                    text: i18n("Remove Track")
                                    icon.name: "list-remove"
                                    icon.color: "white"
                                    onTriggered: UserPlaylistModel.remove(delegateItem.videoId)
                                }
                            ]
                        }
                    }
                }
                Kirigami.Separator {
                    color: "white"
                    opacity: 0.3
                    Layout.fillWidth: true
                }
                
                RowLayout{
                    Layout.topMargin: Kirigami.Units.largeSpacing
                    Layout.bottomMargin: Kirigami.Units.largeSpacing
                    spacing: Kirigami.Units.largeSpacing
                    Item {
                        Layout.fillWidth: true
                    }
                    
                    ToolButton {
                        Layout.preferredHeight: Kirigami.Units.gridUnit * 2.5
                        Layout.maximumWidth: height
                        Layout.preferredWidth: height
                        
                        text: clearPlaylistButton.text
                        icon: clearPlaylistButton.icon
                        display: clearPlaylistButton.display
                        
                        onClicked: {
                            UserPlaylistModel.clearExceptCurrent()
                        }
                        
                        Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                        Kirigami.Theme.inherit: false
                    }
                    
                    ToolButton {
                        Layout.preferredHeight: Kirigami.Units.gridUnit * 2.5
                        Layout.maximumWidth: height
                        Layout.preferredWidth: height
                        
                        onClicked: {
                            UserPlaylistModel.shufflePlaylist()
                        }
                        
                        icon: shuffleButton.icon
                        text: shuffleButton.text
                        display: shuffleButton.display
                        Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                        Kirigami.Theme.inherit: false
                    }
                    
                    Item {
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }
}
