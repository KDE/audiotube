// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.1
import QtQuick.Controls 2.12 as Controls
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.19 as Kirigami

import org.kde.ytmusic 1.0
import "dialogs"

Kirigami.ScrollablePage {
    objectName: "libraryPage"
    Kirigami.Theme.colorSet: Kirigami.Theme.View
    title: "AudioTube"
    readonly property bool isWidescreen: width >= Kirigami.Units.gridUnit * 30

    rightPadding: 0
    leftPadding: 0

    SongMenu {
        id: menu
    }
    ColumnLayout {
        RowLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.MediumSpacing
            Kirigami.Heading {
                text: i18n("Favourites")
                Layout.alignment: Qt.AlignLeft
                leftPadding: 15
            }

            Controls.ToolButton {
                visible: isWidescreen
                text: i18n("Play")
                icon.name: "media-playback-start"
                onClicked: UserPlaylistModel.playFavourites(Library.favourites, false)
            }

            Controls.ToolButton {
                visible: isWidescreen
                text: i18n("Shuffle")
                icon.name: "shuffle"
                onClicked: UserPlaylistModel.playFavourites(Library.favourites, true)
            }

            // Spacer
            Item {
                visible: !isWidescreen
                Layout.fillWidth: true
            }

            Controls.ToolButton {
                Layout.fillHeight: true
                icon.name: "view-more-symbolic"
                onPressed: Kirigami.Settings.isMobile? favDrawer.open() : favMenu.popup()
                Controls.Menu {
                    id: favMenu
                    Controls.MenuItem {
                        visible: !isWidescreen
                        text: i18n("Play")
                        icon.name: "media-playback-start"
                        onTriggered: UserPlaylistModel.playFavourites(Library.favourites, false)
                    }
                    Controls.MenuItem {
                        visible: !isWidescreen
                        text: i18n("Shuffle")
                        icon.name: "shuffle"
                        onTriggered: UserPlaylistModel.playFavourites(Library.favourites, true)
                    }
                    Controls.MenuItem {
                        text: i18n("Append to queue")
                        icon.name: "media-playlist-append"
                        onTriggered: UserPlaylistModel.appendFavourites(Library.favourites,false)
                    }
                }

                BottomDrawer{
                    id: favDrawer
                    drawerContentItem: ColumnLayout {
                        Kirigami.BasicListItem{
                            label: i18n("Play")
                            icon: "media-playback-start"
                            onClicked: {
                                UserPlaylistModel.playFavourites(Library.favourites, false)
                                favDrawer.close()
                            }
                        }
                        Kirigami.BasicListItem{
                            label: i18n("Shuffle")
                            icon: "shuffle"
                            onClicked: {
                                UserPlaylistModel.playFavourites(Library.favourites, true)
                                favDrawer.close()
                            }
                        }
                        Kirigami.BasicListItem{
                            label: i18n("Append to queue")
                            icon: "media-playlist-append"
                            onClicked: {
                                UserPlaylistModel.appendFavourites(Library.favourites,false)
                                favDrawer.close()
                            }
                        }
                        Item{
                            Layout.fillHeight: true
                        }
                    }
                }

            }
            Item {
                visible: isWidescreen
                Layout.fillWidth: true
            }
            Controls.ToolButton {
                text: i18n("Show All")
                Layout.alignment: Qt.AlignRight
                icon.name: "arrow-right"
                onClicked: {pageStack.push("qrc:/PlaybackHistory.qml", {
                      "title": i18n("Favourites"),
                      "objectName": "favourites"
                  })}
            }
        }

        Kirigami.Icon {
            id: favouritesPlaceholder

            visible: mostPlayedRepeater.count == 0
            Layout.margins: 20
            isMask: true
            opacity:0.4
            color: Kirigami.Theme.hoverColor
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            implicitWidth: 190
            implicitHeight: 190

            source: "qrc:/resources/favourites_placeholder.svg"

            Controls.Label {
                visible: favouriteRepeater.count === 0
                color: Kirigami.Theme.disabledTextColor
                text: i18n("No Favourites Yet")

                font {
                    bold: true
                    pointSize: 15
                }

                anchors.centerIn: favouritesPlaceholder
            }
        }

        Controls.ScrollView {
            leftPadding: 15
            rightPadding: 25
            Layout.fillWidth: true
            RowLayout {
                spacing: 20
                Repeater {
                    id: favouriteRepeater
                    Layout.fillWidth: true
                    model: Library.favourites
                    delegate: ColumnLayout {
                        id: delegateItem
                        required property string title
                        required property var artists
                        required property string artistsDisplayString
                        required property string videoId
                        required property int index

                        Layout.fillWidth: false
                        Layout.maximumWidth: 200

                        Kirigami.ShadowedRectangle {
                            color: Kirigami.Theme.backgroundColor
                            id: favCover
                            MouseArea {
                                id: favArea
                                anchors.fill: parent
                                onClicked: play(delegateItem.videoId)
                                hoverEnabled: !Kirigami.Settings.hasTransientTouchInput
                                onEntered: {
                                    if (!Kirigami.Settings.hasTransientTouchInput){
                                        favSelected.visible = true
                                        favTitle.color = Kirigami.Theme.hoverColor
                                        favSubtitle.color = Kirigami.Theme.hoverColor
                                        favTitle.font.bold = true
                                        playAnimationPosition.running = true
                                        playAnimationOpacity.running = true
                                    }

                                }

                                onExited:{
                                    favSelected.visible = false
                                    favTitle.color = Kirigami.Theme.textColor
                                    favSubtitle.color = Kirigami.Theme.disabledTextColor
                                    favTitle.font.bold = false
                                }
                            }
                            Layout.margins: 5

                            width: 200
                            height: 200
                            radius: 10
                            shadow.size: 15
                            shadow.xOffset: 5
                            shadow.yOffset: 5
                            shadow.color: Qt.rgba(0, 0, 0, 0.2)
                            ThumbnailSource {
                                id: thumbnailSource
                                videoId: delegateItem.videoId
                            }
                            RoundedImage {
                                source: thumbnailSource.cachedPath
                                height: 200
                                width: height
                                radius: 10
                            }
                            Rectangle {
                                id: favSelected

                                Rectangle {
                                    anchors.fill: parent
                                    color: Kirigami.Theme.hoverColor
                                    radius: 10
                                    opacity: 0.2
                                }
                                Item{
                                    height: parent.height
                                    width: parent.width
                                    NumberAnimation on opacity{
                                        id: playAnimationOpacity
                                        easing.type: Easing.OutCubic
                                        running: false
                                        from: 0; to: 1
                                    }
                                    NumberAnimation on y {
                                        id: playAnimationPosition
                                        easing.type: Easing.OutCubic
                                        running: false
                                        from: 20; to: 0
                                        duration: 100
                                    }
                                    Rectangle {
                                        height: 45
                                        width: 45
                                        radius: 50
                                        color: Kirigami.Theme.hoverColor
                                        opacity: 0.8
                                        anchors.centerIn: parent


                                    }
                                    Kirigami.Icon {
                                        x: 100 - 0.43 * height
                                        y: 100 - 0.5  * height
                                        color: "white"
                                        source: "media-playback-start"
                                    }
                                }
                                visible: false
                                anchors.fill: parent

                                radius: 9

                                border.color: Kirigami.Theme.hoverColor
                                border.width: 2
                                color: "transparent"
                            }
                        }

                        RowLayout {
                            ColumnLayout {
                                Controls.Label {
                                    id:favTitle
                                    text: delegateItem.title
                                    Layout.maximumWidth: 200
                                    Layout.fillWidth: true
                                    leftPadding: 5
                                    elide: Text.ElideRight

                                }
                                Controls.Label {
                                    id: favSubtitle
                                    Layout.fillWidth: true
                                    Layout.maximumWidth: 200
                                    leftPadding: 5
                                    color: Kirigami.Theme.disabledTextColor
                                    text: delegateItem.artistsDisplayString
                                    elide: Text.ElideRight
                                }
                            }
                            Controls.ToolButton {
                                Layout.fillHeight: true
                                icon.name: "overflow-menu"
                                onPressed: menu.openForSong(delegateItem.videoId,
                                                               delegateItem.title,
                                                               delegateItem.artists,
                                                               delegateItem.artistsDisplayString)
                            }

                        }
                        Item {
                            height: 5
                        }
                    }
                }
            }
        }
        Item {
            height: 20
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.MediumSpacing

            Kirigami.Heading {
                text: i18n("Most played")
                Layout.alignment: Qt.AlignLeft
                leftPadding: 15
            }

            Controls.ToolButton {
                visible: isWidescreen
                text: i18n("Play")
                icon.name: "media-playback-start"
                onClicked: UserPlaylistModel.playPlaybackHistory(Library.mostPlayed, false)
            }

            Controls.ToolButton {
                visible: isWidescreen
                text: i18n("Shuffle")
                icon.name: "shuffle"
                onClicked: UserPlaylistModel.playPlaybackHistory(Library.mostPlayed, true)
            }

            // Spacer
            Item {
                visible: !isWidescreen
                Layout.fillWidth: true
            }

            Controls.ToolButton {
                Layout.fillHeight: true
                icon.name: "view-more-symbolic"
                onPressed: Kirigami.Settings.isMobile? recDrawer.open() : recMenu.popup()
                Controls.Menu {
                    id: recMenu
                    Controls.MenuItem {
                        visible: !isWidescreen
                        text: i18n("Play")
                        icon.name: "media-playback-start"
                        onTriggered: UserPlaylistModel.playPlaybackHistory(Library.mostPlayed, false)
                    }
                    Controls.MenuItem {
                        visible: !isWidescreen
                        text: i18n("Shuffle")
                        icon.name: "shuffle"
                        onTriggered: UserPlaylistModel.playPlaybackHistory(Library.mostPlayed, true)
                    }
                    Controls.MenuItem {
                        text: i18n("Append to queue")
                        icon.name: "media-playlist-append"
                        onTriggered: UserPlaylistModel.appendPlaybackHistory(Library.mostPlayed, false)
                    }
                }

                BottomDrawer{
                    id: recDrawer
                    drawerContentItem: ColumnLayout {
                        Kirigami.BasicListItem{
                            label: i18n("Play")
                            icon: "media-playback-start"
                            onClicked: {
                                UserPlaylistModel.playPlaybackHistory(Library.mostPlayed, false)
                                recDrawer.close()
                            }

                        }
                        Kirigami.BasicListItem{
                            label: i18n("Shuffle")
                            icon: "shuffle"
                            onClicked: {
                                UserPlaylistModel.playPlaybackHistory(Library.mostPlayed, true)
                                recDrawer.close()
                            }
                        }
                        Kirigami.BasicListItem{
                            label: i18n("Append to queue")
                            icon: "media-playlist-append"
                            onClicked: {
                                UserPlaylistModel.appendPlaybackHistory(Library.mostPlayed, false)
                                recDrawer.close()
                            }
                        }
                        Item{
                            Layout.fillHeight: true
                        }

                    }
                }

            }
            Item {
                visible: isWidescreen
                Layout.fillWidth: true
            }
            Controls.ToolButton {
                text: i18n("Show All")
                Layout.alignment: Qt.AlignRight
                icon.name: "arrow-right"
                onClicked: {pageStack.push("qrc:/PlaybackHistory.qml", {
                      "title": i18n("Played Songs"),
                      "objectName": "history"
                  })}
            }
        }
        Kirigami.Icon {
            visible: mostPlayedRepeater.count == 0
            Layout.margins: 20
            isMask: true
            opacity:0.4
            color: Kirigami.Theme.hoverColor
            id:playedPlaceholder
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            implicitWidth: 190
            implicitHeight: 190
            source: "qrc:/resources/played_placeholder.svg"

            Controls.Label {
                visible: mostPlayedRepeater.count == 0
                color: Kirigami.Theme.disabledTextColor
                anchors.centerIn:playedPlaceholder
                font.bold: true
                font.pointSize: 15
                text: i18n("No Songs Played Yet")
            }
        }

        Controls.ScrollView {
            leftPadding: 15
            rightPadding: 25
            Layout.fillWidth: true
            RowLayout {
                spacing: 20

                Repeater {
                    id: mostPlayedRepeater
                    Layout.fillWidth: true
                    model: Library.mostPlayed
                    delegate: ColumnLayout {
                        id: mpdelegateItem
                        required property string title
                        required property var artists
                        required property string artistsDisplayString
                        required property string videoId

                        Layout.fillWidth: false
                        Layout.maximumWidth: 200

                        Kirigami.ShadowedRectangle {
                            color: Kirigami.Theme.backgroundColor
                            id: recCover
                            MouseArea {
                                id: recArea
                                anchors.fill: parent
                                onClicked: play(mpdelegateItem.videoId)
                                hoverEnabled: !Kirigami.Settings.hasTransientTouchInput
                                onEntered: {
                                    if (!Kirigami.Settings.hasTransientTouchInput)
                                        recSelected.visible = true
                                        recTitle.color = Kirigami.Theme.hoverColor
                                        recSubtitle.color = Kirigami.Theme.hoverColor
                                        recTitle.font.bold = true
                                        playAnimationPositionRec.running = true
                                        playAnimationOpacityRec.running = true
                                }
                                onExited:{
                                    recSelected.visible = false
                                    recTitle.color = Kirigami.Theme.textColor
                                    recSubtitle.color = Kirigami.Theme.disabledTextColor
                                    recTitle.font.bold = false
                                }

                            }
                            Layout.margins: 5

                            width: 200
                            height: 200
                            radius: 10
                            shadow.size: 15
                            shadow.xOffset: 5
                            shadow.yOffset: 5
                            shadow.color: Qt.rgba(0, 0, 0, 0.2)
                            ThumbnailSource {
                                id: mpthumbnailSource
                                videoId: mpdelegateItem.videoId
                            }
                            RoundedImage {
                                source: mpthumbnailSource.cachedPath
                                height: 200
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
                                Item{
                                    height: parent.height
                                    width: parent.width
                                    NumberAnimation on opacity{
                                        id: playAnimationOpacityRec
                                        easing.type: Easing.OutCubic
                                        running: false
                                        from: 0; to: 1
                                    }
                                    NumberAnimation on y {
                                        id: playAnimationPositionRec
                                        easing.type: Easing.OutCubic
                                        running: false
                                        from: 20; to: 0
                                        duration: 100
                                    }
                                    Rectangle {
                                        height: 45
                                        width: 45
                                        radius: 50
                                        color: Kirigami.Theme.hoverColor
                                        opacity: 0.8
                                        anchors.centerIn: parent


                                    }
                                    Kirigami.Icon {
                                        x: 100 - 0.43 * height
                                        y: 100 - 0.5  * height
                                        color: "white"
                                        source: "media-playback-start"
                                    }
                                }

                                visible: false
                                anchors.fill: parent

                                radius: 9

                                border.color: Kirigami.Theme.hoverColor
                                border.width: 2
                                color: "transparent"
                            }
                        }
                        RowLayout {
                            ColumnLayout {
                                Controls.Label {
                                    id: recTitle
                                    text: mpdelegateItem.title
                                    Layout.maximumWidth: 200
                                    Layout.fillWidth: true
                                    leftPadding: 5
                                    elide: Text.ElideRight

                                }
                                Controls.Label {
                                    id: recSubtitle
                                    Layout.fillWidth: true
                                    Layout.maximumWidth: 200
                                    leftPadding: 5
                                    color: Kirigami.Theme.disabledTextColor
                                    text: mpdelegateItem.artistsDisplayString
                                    elide: Text.ElideRight
                                }
                            }
                            Controls.ToolButton {
                                Layout.fillHeight: true
                                icon.name: "overflow-menu"
                                onPressed: menu.openForSong(mpdelegateItem.videoId,
                                                              mpdelegateItem.title,
                                                              mpdelegateItem.artists,
                                                              mpdelegateItem.artistsDisplayString)
                            }

                        }
                        Item {
                            height: 5
                        }
                    }
                }
            }
        }
        Item {
            height: 20
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.MediumSpacing
            Kirigami.Heading {
                text: i18n("Playlists")
                Layout.alignment: Qt.AlignLeft
                leftPadding: 15
            }


            // Spacer
            Item {
                Layout.fillWidth: true
            }

            Controls.ToolButton {
                text: i18n("Show All")
                Layout.alignment: Qt.AlignRight
                icon.name: "arrow-right"
                onClicked: {pageStack.push("qrc:/LocalPlaylistsPage.qml", {
                      "objectName": "playlists"
                  })}
            }
        }

        Kirigami.Icon {
            id: playlistsPlaceholder

            visible: mostPlayedRepeater.count == 0
            Layout.margins: 20
            isMask: true
            opacity:0.4
            color: Kirigami.Theme.hoverColor
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            implicitWidth: 190
            implicitHeight: 190

            source: "qrc:/resources/playlist_placeholder.svg"

            Controls.Label {
                visible: favouriteRepeater.count === 0
                color: Kirigami.Theme.disabledTextColor
                text: i18n("No Playlists Yet")

                font {
                    bold: true
                    pointSize: 15
                }

                anchors.centerIn: playlistsPlaceholder
            }
        }

        RenamePlaylistDialog{
            id: renamePlaylistDialog
            playlistsModel: localPlaylistsModel
        }
        BottomDrawer{
            id: playlistDrawer
            property var modelData
            drawerContentItem: ColumnLayout {
                Kirigami.BasicListItem{
                    label: i18n("Rename")
                    icon: "edit-entry"
                    onClicked: {
                        renamePlaylistDialog.modelData = playlistDrawer.modelData
                        renamePlaylistDialog.open()
                        playlistDrawer.close()
                    }
                }
                Kirigami.BasicListItem{
                    label: i18n("Delete")
                    icon: "delete"
                    onClicked: {
                        localPlaylistsModel.deletePlaylist(playlistDrawer.modelData.playlistId)
                        playlistDrawer.close()
                    }
                }
            }
        }
        Controls.Menu {
            id: playlistMenu
            property var modelData
            Controls.MenuItem {
                text: i18n("Rename")
                icon.name: "edit-entry"
                onTriggered:{
                    renamePlaylistDialog.modelData = playlistMenu.modelData
                    renamePlaylistDialog.open()
                }
            }
            Controls.MenuItem {
                text: i18n("Delete")
                icon.name: "delete"
                onTriggered:{
                    localPlaylistsModel.deletePlaylist(playlistMenu.modelData.playlistId)
                }
            }
        }

        Controls.ScrollView {
            leftPadding: 15
            rightPadding: 25
            Layout.fillWidth: true
            RowLayout {
                spacing: 20
                Repeater {
                    id: playlistsRepeater
                    Layout.fillWidth: true
                    model: LocalPlaylistsModel {
                        id: localPlaylistsModel
                    }
                    delegate: ColumnLayout {
                        id: playlistDelegate
                        required property var model
                        required property string playlistId
                        required property string title
                        required property string description
                        required property date createdOn
                        required property var thumbnailIds
                        onThumbnailIdsChanged: console.log(thumbnailIds)

                        Layout.fillWidth: false
                        Layout.maximumWidth: 200
                        Layout.preferredWidth: 200
                        Kirigami.ShadowedRectangle {
                            color: Kirigami.Theme.backgroundColor
                            id: playlistsCover
                            MouseArea {
                                id: playlistsArea
                                anchors.fill: parent
                                onClicked: pageStack.push("qrc:/LocalPlaylistPage.qml", {
                                                                     "playlistId": playlistDelegate.playlistId,
                                                                     "title": playlistDelegate.title
                                                                 })
                                hoverEnabled: !Kirigami.Settings.hasTransientTouchInput
                                onEntered: {
                                    if (!Kirigami.Settings.hasTransientTouchInput){
                                        playlistSelected.visible = true
                                        playlistTitle.color = Kirigami.Theme.hoverColor
                                        playlistSubtitle.color = Kirigami.Theme.hoverColor
                                        playlistTitle.font.bold = true
                                    }

                                }

                                onExited:{
                                    playlistSelected.visible = false
                                    playlistTitle.color = Kirigami.Theme.textColor
                                    playlistSubtitle.color = Kirigami.Theme.disabledTextColor
                                    playlistTitle.font.bold = false
                                }
                            }
                            Layout.margins: 5
                            width: 200
                            height: 200
                            radius: 10
                            shadow.size: 15
                            shadow.xOffset: 5
                            shadow.yOffset: 5
                            shadow.color: Qt.rgba(0, 0, 0, 0.2)


                            LocalPlaylistsModel{id:localPlaylistModel}

                            ThumbnailSource {
                                id: thumbnailSource1
                                videoId: thumbnailIds[0]
                            }
                            ThumbnailSource {
                                id: thumbnailSource2
                                videoId: thumbnailIds[1] ?? thumbnailIds[0]
                            }
                            ThumbnailSource {
                                id: thumbnailSource3
                                videoId: thumbnailIds[2] ?? thumbnailIds[0]
                            }
                            ThumbnailSource {
                                id: thumbnailSource4
                                videoId: thumbnailIds[3] ?? thumbnailIds[0]
                            }
                            PlaylistCover {
                                source1: thumbnailSource1.cachedPath
                                source2: thumbnailSource2.cachedPath
                                source3: thumbnailSource3.cachedPath
                                source4: thumbnailSource4.cachedPath
                                title: playlistDelegate.title
                                height: 200
                                width: height
                                radius: 10
                            }

                            Rectangle {
                                id: playlistSelected

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

                        RowLayout {
                            ColumnLayout {
                                Controls.Label {
                                    id: playlistTitle
                                    text: playlistDelegate.title
                                    Layout.maximumWidth: 200
                                    Layout.fillWidth: true
                                    leftPadding: 5
                                    elide: Text.ElideRight

                                }
                                Controls.Label {
                                    id: playlistSubtitle
                                    Layout.fillWidth: true
                                    Layout.maximumWidth: 200
                                    leftPadding: 5
                                    color: Kirigami.Theme.disabledTextColor
                                    text: playlistDelegate.description
                                    elide: Text.ElideRight
                                }
                            }
                            Controls.ToolButton {
                                Layout.fillHeight: true
                                icon.name: "overflow-menu"
                                onClicked:{
                                    playlistMenu.modelData = playlistDelegate.model
                                    playlistDrawer.modelData = playlistDelegate.model
                                    Kirigami.Settings.isMobile? playlistDrawer.open() : playlistMenu.popup()
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
