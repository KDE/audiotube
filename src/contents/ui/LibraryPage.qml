// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Window
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigami.delegates as KirigamiDelegates
import org.kde.kirigamiaddons.components as Components

import org.kde.audiotube

Kirigami.ScrollablePage {
    id: libraryPage
    objectName: "libraryPage"
    Kirigami.Theme.colorSet: Kirigami.Theme.View
    title: "AudioTube"
    readonly property bool isWidescreen: width >= Kirigami.Units.gridUnit * 30

    leftPadding: 0
    rightPadding: 0

    ColumnLayout {
        RowLayout {
            Layout.fillWidth: true
            Layout.maximumWidth: libraryPage.width

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

                Components.BottomDrawer {
                    id: favDrawer

                    parent: applicationWindow().overlay

                    drawerContentItem: ColumnLayout {
                        KirigamiDelegates.SubtitleDelegate {
                            Layout.fillWidth: true

                            text: i18n("Play")
                            icon.name: "media-playback-start"
                            onClicked: {
                                UserPlaylistModel.playFavourites(Library.favourites, false)
                                favDrawer.close()
                            }
                        }
                        KirigamiDelegates.SubtitleDelegate {
                            Layout.fillWidth: true

                            text: i18n("Shuffle")
                            icon.name: "shuffle"
                            onClicked: {
                                UserPlaylistModel.playFavourites(Library.favourites, true)
                                favDrawer.close()
                            }
                        }
                        KirigamiDelegates.SubtitleDelegate {
                            Layout.fillWidth: true

                            text: i18n("Append to queue")
                            icon.name: "media-playlist-append"
                            onClicked: {
                                UserPlaylistModel.appendFavourites(Library.favourites,false)
                                favDrawer.close()
                            }
                        }
                        Item {
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
                onClicked: {
                    pageStack.push(pool.loadPageWithProperties("qrc:/qt/qml/org/kde/audiotube/contents/ui/PlaybackHistory.qml#favourites", {
                        "title": i18n("Favourites"),
                        "objectName": "favourites"
                    }))
                }
            }
        }

        Kirigami.Icon {
            id: favouritesPlaceholder

            visible: favouriteRepeater.count === 0
            Layout.margins: 20
            isMask: true
            opacity: 0.4
            color: Kirigami.Theme.hoverColor
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            implicitWidth: 190
            implicitHeight: 190

            source: "qrc:/qt/qml/org/kde/audiotube/resources/favourites_placeholder.svg"

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

        HorizontalCoverView {
            id: favouriteRepeater

            Layout.fillWidth: false
            Layout.maximumWidth: libraryPage.flickable.width

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

                ThumbnailSource {
                    id: thumbnailSource
                    videoId: delegateItem.videoId
                }

                AlbumCoverItem {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    onClicked: play(delegateItem.videoId)
                    onOptionsClicked: menu.openForSong(delegateItem.videoId,
                                                              delegateItem.title,
                                                              delegateItem.artists,
                                                              delegateItem.artistsDisplayString)

                    title: delegateItem.title
                    subtitle: delegateItem.artistsDisplayString

                    albumCover: RoundedImage {
                        source: thumbnailSource.cachedPath
                        anchors.fill: parent
                        radius: 10
                    }
                }

                Item {
                    height: 5
                }
            }
        }
        Item {
            height: 20
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.maximumWidth: libraryPage.flickable.width

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

                Components.BottomDrawer{
                    id: recDrawer

                    parent: applicationWindow().overlay

                    drawerContentItem: ColumnLayout {
                        KirigamiDelegates.SubtitleDelegate{
                            text: i18n("Play")
                            icon.name: "media-playback-start"
                            onClicked: {
                                UserPlaylistModel.playPlaybackHistory(Library.mostPlayed, false)
                                recDrawer.close()
                            }

                        }
                        KirigamiDelegates.SubtitleDelegate{
                            text: i18n("Shuffle")
                            icon.name: "shuffle"
                            onClicked: {
                                UserPlaylistModel.playPlaybackHistory(Library.mostPlayed, true)
                                recDrawer.close()
                            }
                        }
                        KirigamiDelegates.SubtitleDelegate{
                            text: i18n("Append to queue")
                            icon.name: "media-playlist-append"
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
                onClicked: {
                    pageStack.push(pool.loadPageWithProperties("qrc:/qt/qml/org/kde/audiotube/contents/ui/PlaybackHistory.qml#history", {
                        "title": i18n("Played Songs"),
                        "objectName": "history"
                    }))
                }
            }
        }
        Kirigami.Icon {
            visible: mostPlayedRepeater.count === 0
            Layout.margins: 20
            Layout.maximumWidth: libraryPage.flickable.width
            isMask: true
            opacity: 0.4
            color: Kirigami.Theme.hoverColor
            id: playedPlaceholder
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            implicitWidth: 190
            implicitHeight: 190
            source: "qrc:/qt/qml/org/kde/audiotube/resources/played_placeholder.svg"

            Controls.Label {
                visible: mostPlayedRepeater.count === 0
                color: Kirigami.Theme.disabledTextColor
                anchors.centerIn: playedPlaceholder
                font.bold: true
                font.pointSize: 15
                text: i18n("No Songs Played Yet")
            }
        }

        HorizontalCoverView {
            id: mostPlayedRepeater

            Layout.fillWidth: false
            Layout.maximumWidth: libraryPage.flickable.width

            model: Library.mostPlayed
            delegate: ColumnLayout {
                id: mpdelegateItem
                required property string title
                required property var artists
                required property string artistsDisplayString
                required property string videoId

                Layout.fillWidth: false
                Layout.maximumWidth: 200

                ThumbnailSource {
                    id: mpthumbnailSource
                    videoId: mpdelegateItem.videoId
                }

                AlbumCoverItem {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    onClicked: play(mpdelegateItem.videoId)
                    onOptionsClicked: menu.openForSong(mpdelegateItem.videoId,
                                                              mpdelegateItem.title,
                                                              mpdelegateItem.artists,
                                                              mpdelegateItem.artistsDisplayString)

                    title: mpdelegateItem.title
                    subtitle: mpdelegateItem.artistsDisplayString

                    albumCover: RoundedImage {
                        source: mpthumbnailSource.cachedPath
                        anchors.fill: parent
                        radius: 10
                    }
                }

                Item {
                    height: 5
                }
            }
        }
        Item {
            height: 20
        }
        RowLayout {
            Layout.fillWidth: true
            Layout.maximumWidth: libraryPage.flickable.width

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
                onClicked: {pageStack.push("qrc:/qt/qml/org/kde/audiotube/contents/ui/LocalPlaylistsPage.qml", {
                      "objectName": "playlists"
                  })}
            }
        }

        Kirigami.Icon {
            id: playlistsPlaceholder

            visible: playlistsRepeater.count === 0
            Layout.margins: 20
            isMask: true
            opacity:0.4
            color: Kirigami.Theme.hoverColor
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            implicitWidth: 190
            implicitHeight: 190

            source: "qrc:/qt/qml/org/kde/audiotube/resources/playlist_placeholder.svg"

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
        Components.BottomDrawer{
            id: playlistDrawer

            parent: applicationWindow().overlay

            property var modelData
            drawerContentItem: ColumnLayout {
                KirigamiDelegates.SubtitleDelegate{
                    text: i18n("Rename")
                    icon.name: "edit-entry"
                    onClicked: {
                        renamePlaylistDialog.modelData = playlistDrawer.modelData
                        renamePlaylistDialog.open()
                        playlistDrawer.close()
                    }
                }
                KirigamiDelegates.SubtitleDelegate{
                    text: i18n("Delete")
                    icon.name: "delete"
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

        HorizontalCoverView {
            id: playlistsRepeater

            Layout.fillWidth: true
            Layout.maximumWidth: libraryPage.flickable.width

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

                Layout.fillWidth: false
                Layout.maximumWidth: 200
                Layout.preferredWidth: 200

                AlbumCoverItem {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    title: playlistDelegate.title
                    subtitle: playlistDelegate.description
                    showIcon: false

                    LocalPlaylistsModel{id:localPlaylistModel}

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
                    albumCover: PlaylistCover {
                        source1: thumbnailSource1.cachedPath
                        source2: thumbnailSource2.cachedPath
                        source3: thumbnailSource3.cachedPath
                        source4: thumbnailSource4.cachedPath
                        title: playlistDelegate.title
                        height: 200
                        width: height
                        radius: 10
                    }

                    onClicked: pageStack.push("qrc:/qt/qml/org/kde/audiotube/contents/ui/LocalPlaylistPage.qml", {
                        "playlistId": playlistDelegate.playlistId,
                        "title": playlistDelegate.title
                    })

                    onOptionsClicked:{
                        playlistMenu.modelData = playlistDelegate.model
                        playlistDrawer.modelData = playlistDelegate.model
                        Kirigami.Settings.isMobile? playlistDrawer.open() : playlistMenu.popup()
                    }
                }
            }
        }
    }

    SongMenu {
        id: menu
    }
}
