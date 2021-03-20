import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14 as Controls
import org.kde.kirigami 2.14 as Kirigami

import org.kde.ytmusic 1.0

Kirigami.ScrollablePage {
    title: i18n("Youtube Music")

    header: Controls.Control {
        padding: Kirigami.Units.largeSpacing
        contentItem: Kirigami.SearchField {
            onAccepted: {
                searchModel.searchQuery = text
            }
        }
    }

    ListView {
        model: SearchModel {
            id: searchModel

            onOpenAlbum: (browseId) => {
                pageStack.push("qrc:/AlbumPage.qml", {
                    "browseId": browseId
                })
            }

            onOpenArtist: (channelId) => {
                pageStack.push("qrc:/ArtistPage.qml", {
                    "channelId": channelId
                })
            }

            onOpenPlaylist: (browseId) => {

            }

            onOpenSong: (videoId) => {
                play(videoId)
            }

            onOpenVideo: (videoId) => {
                showPassiveNotification(i18n("Video playback is not supported yet."),
                                    "long",
                                    i18n("Play only audio"),
                                    function() {
                                        play(videoId)
                                    })
            }
        }
        delegate: Kirigami.SwipeListItem {
            id: delegateItem
            required property int index
            required property string title
            required property int type
            required property string videoId
            required property var artists

            RowLayout {
                Layout.fillHeight: true
                Kirigami.Icon {
                    Layout.fillHeight: true
                    source: {
                        switch (type) {
                        case SearchModel.Artist:
                            return "view-media-artist"
                        case SearchModel.Album:
                            return "media-album-cover"
                        case SearchModel.Playlist:
                            return "view-media-playlist"
                        case SearchModel.Song:
                            return "emblem-music-symbolic"
                        case SearchModel.Video:
                            return "emblem-videos-symbolic"
                        }
                    }
                }

                Controls.Label {
                    Layout.fillWidth: true
                    text: title
                    elide: Qt.ElideRight
                }
            }

            actions: [
                Kirigami.Action {
                    icon.name: "go-next"
                    text: i18n("Play next")
                    visible: type === SearchModel.Song
                    onTriggered: PlaylistModel.playNext(delegateItem.videoId, delegateItem.title, delegateItem.artists)
                },
                Kirigami.Action {
                    icon.name: "media-playlist-append"
                    text: i18n("Add to playlist")
                    visible: type === SearchModel.Song
                    onTriggered: PlaylistModel.append(delegateItem.videoId, delegateItem.title, delegateItem.artists)
                }
            ]

            onClicked: searchModel.triggerItem(index)
        }
        Controls.BusyIndicator {
            anchors.centerIn: parent
            visible: searchModel.loading
        }
    }
}
