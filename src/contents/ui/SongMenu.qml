// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as Controls
import org.kde.audiotube
import org.kde.kirigami as Kirigami
import org.kde.kirigami.delegates as KirigamiDelegates
import org.kde.kirigamiaddons.components as Components

import QtQuick.Layouts

Components.ConvergentContextMenu {
    id: root

    property string videoId: ''
    property string songTitle: ''
    property var artists
    property string artistsDisplayString: ''

    readonly property QtObject wasPlayedWatcher: root.videoId !== '' ? Library.wasPlayedWatcher(root.videoId) : null
    readonly property QtObject favouriteWatcher: root.videoId !== '' ? Library.favouriteWatcher(root.videoId) : null
    property var parentItem

    readonly property PlaylistDialog playlistDialog: PlaylistDialog {}

    function openForSong(videoId: string , songTitle: string , artists: var, artistsDisplayString: string): void {
        root.videoId = videoId;
        root.songTitle = songTitle;
        root.artists = artists;
        root.artistsDisplayString = artistsDisplayString;
        root.popup(root.parentItem);
    }

    headerContentItem: RowLayout {
        spacing: Kirigami.Units.gridUnit

        ThumbnailSource {
            id: thumbnailSource
            videoId: root.videoId
        }

        Kirigami.ShadowedRectangle {
            Layout.rightMargin: Kirigami.Units.smallSpacing
            Layout.preferredWidth: 60
            Layout.preferredHeight: 60

            color: Kirigami.Theme.backgroundColor
            radius: Kirigami.Units.cornerRadius
            shadow {
                size: 15
                xOffset: 5
                yOffset: 5
                color: Qt.rgba(0, 0, 0, 0.2)
            }

            RoundedImage {
                source: thumbnailSource.cachedPath
                anchors.fill: parent
                radius: Kirigami.Units.cornerRadius
            }
        }

        ColumnLayout {
            spacing: Kirigami.Units.smallSpacing

            Controls.Label {
                text: root.songTitle
                elide: Text.ElideRight
                Layout.fillWidth: true
            }

            Controls.Label {
                text: root.artistsDisplayString
                color: Kirigami.Theme.disabledTextColor
                elide: Text.ElideRight
                Layout.fillWidth: true
            }
        }
    }

    Controls.Action {
        text: i18n("Play Next")
        icon.name: "go-next"
        onTriggered: {
            UserPlaylistModel.playNext(root.videoId, root.songTitle, root.artists)
            root.close()
        }
    }

    Controls.Action {
        text: i18n("Add to queue")
        icon.name: "media-playlist-append"
        onTriggered: {
            UserPlaylistModel.append(root.videoId, root.songTitle, root.artists)
            root.close()
        }
    }

    Kirigami.Action {
        visible: root.favouriteWatcher
        text: root.favouriteWatcher?.isFavourite ? i18n("Remove from Favorites"): i18n("Add to Favorites")
        icon.name: root.favouriteWatcher?.isFavourite ? "starred-symbolic" : "non-starred-symbolic"
        onTriggered: {
            if (favouriteWatcher.isFavourite) {
                Library.removeFavourite(root.videoId)
            } else {
                Library.addFavourite(root.videoId, root.songTitle, root.artistsDisplayString, "")
            }
            root.close()
        }
    }

    Kirigami.Action {
        text: i18n("Remove from History")
        icon.name: "list-remove"
        onTriggered: {
            Library.removePlaybackHistoryItem(root.videoId)
            root.close()
        }
        visible: root.wasPlayedWatcher?.wasPlayed ?? false
    }

    Kirigami.Action {
        text: i18n("Add to playlist")
        icon.name: "media-playlist-append"
        onTriggered: {
            playlistDialog.open()
            root.close()
        }
    }

    Controls.Action {
        text: i18n("Share Song")
        icon.name: "emblem-shared-symbolic"
        onTriggered: {
            openShareMenu(root.songTitle, "https://music.youtube.com/watch?v=" + root.videoId)
            root.close()
        }
    }
}
