﻿// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef YTMUSIC_H
#define YTMUSIC_H

#include <string>
#include <optional>
#include <map>
#include <variant>
#include <vector>
#include <memory>

struct YTMusicPrivate;

namespace meta {
struct Thumbnail {
    std::string url;
    int width;
    int height;

    bool operator<(const Thumbnail other) const {
        return height < other.height;
    }
};
struct Artist {
    std::string name;
    std::optional<std::string> id;
};
struct Album {
    std::string name;
    std::optional<std::string> id;
};
}


namespace search {
///
/// Base class for Video and Song, which share most attributes.
///
struct Media {
    std::string video_id;
    std::string title;
    std::vector<meta::Artist> artists;
    std::string duration;
};

struct Video : public Media {
    std::string views;
};

struct Playlist {
    std::string browse_id;
    std::string title;
    std::string author;
    std::string item_count;
};

struct Song : public Media {
    meta::Album album;
    bool is_explicit;
};

struct Album {
    std::string browse_id;
    std::string title;
    std::string type;
    std::vector<meta::Artist> artists;
    std::string year;
    bool is_explicit;
};

struct Artist {
    std::string browse_id;
    std::string artist;
    std::optional<std::string> shuffle_id;
    std::optional<std::string> radio_id;
};

using SearchResultItem = std::variant<Video, Playlist, Song, Album, Artist>;
};


namespace artist {
struct Artist {
    template<typename T>
    struct Section {
        std::optional<std::string> browse_id;
        std::vector<T> results;
        std::optional<std::string> params;
    };

    struct Song {
        struct Album {
            std::string name;
            std::string id;
        };

        std::string video_id;
        std::string title;
        std::vector<meta::Thumbnail> thumbnails;
        std::vector<meta::Artist> artist;
        Album album;
    };

    struct Album {
        std::string title;
        std::vector<meta::Thumbnail> thumbnails;
        std::string year;
        std::string browseId;
        std::optional<std::string> type;
    };

    struct Video {
        std::string title;
        std::vector<meta::Thumbnail> thumbnails;
        std::optional<std::string> views;
        std::string videoId;
        std::string playlist_id;
    };

    struct Single {
        std::string title;
        std::vector<meta::Thumbnail> thumnnails;
        std::string year;
        std::string browseId;
    };

    std::optional<std::string> description;
    std::optional<std::string> views;
    std::string name;
    std::string channel_id;
    std::string subscribers;
    bool subscribed;
    std::vector<meta::Thumbnail> thumbnails;
    std::optional<Section<Song>> songs;
    std::optional<Section<Album>> albums;
    std::optional<Section<Single>> singles;
    std::optional<Section<Video>> videos;
};
}

namespace album {
    struct Track {
        std::string index;
        std::string title;
        std::string artists;
        std::optional<std::string> video_id;
        std::optional<std::string> length_ms;
        std::optional<std::string> like_status;
    };

    struct Album {
        struct ReleaseDate {
            int year;
            int month;
            int day;
        };

        std::string title;
        std::string track_count;
        std::string duration_ms;
        std::string playlist_id;
        ReleaseDate release_date;
        std::string description;
        std::vector<meta::Thumbnail> thumbnails;
        std::vector<Track> tracks;
    };
}

namespace song {
struct Song {
    struct Thumbnail {
        std::vector<meta::Thumbnail> thumbnails;
    };

    std::string video_id;
    std::string title;
    std::string length;
    std::vector<std::string> keywords;
    std::string channel_id;
    bool is_owner_viewer;
    std::string short_description;
    bool is_crawlable;
    Thumbnail thumbnail;
    float average_rating;
    bool allow_ratings;
    std::string view_count;
    std::string author;
    bool is_private;
    bool is_unplugged_corpus;
    bool is_live_content;
    std::string provider;
    std::vector<std::string> artists;
    std::string copyright;
    std::vector<std::string> production;
    std::string release;
    std::string category;
};
}

namespace playlist {
struct Track {
    std::optional<std::string> video_id;
    std::string title;
    std::vector<meta::Artist> artists;
    std::optional<meta::Album> album;
    std::string duration;
    std::optional<std::string> like_status;
    std::vector<meta::Thumbnail> thumbnails;
    bool is_available;
    bool is_explicit;
};

struct Playlist {
    std::string id;
    std::string privacy;
    std::string title;
    std::vector<meta::Thumbnail> thumbnails;
    meta::Artist author;
    std::string year;
    std::string duration;
    int track_count;
    std::vector<Track> tracks;
};
}

namespace video_info {
struct Format {
    int quality;
    std::string url;
    std::string vcodec;
    std::string acodec;

    // More, but not interesting for us right now
};

struct VideoInfo {
    std::string id;
    std::string title;
    std::vector<Format> formats;

    // More, but not interesting for us right now
};
}

namespace watch {
struct Playlist {
    struct Track {
        std::string title;
        std::string length;
        std::string videoId;
        std::string playlistId;
        std::vector<meta::Thumbnail> thumbnail;
        std::optional<std::string> like_status;
        std::vector<meta::Artist> artists;
        meta::Album album;
    };

    const std::vector<Track> tracks;
    const std::optional<std::string> lyrics;
};
}

class YTMusic
{
public:
    YTMusic(const std::optional<std::string> &auth = std::nullopt,
            const std::optional<std::string> &user = std::nullopt,
            const std::optional<bool> requests_session = std::nullopt,
            const std::optional<std::map<std::string, std::string>> &proxies = std::nullopt,
            const std::string &language = "en");

    ~YTMusic();

    std::vector<search::SearchResultItem> search(const std::string &query,
                                                 const std::optional<std::string> &filter = std::nullopt,
                                                 const int limit = 20,
                                                 const bool ignore_spelling = false) const;

    /// https://ytmusicapi.readthedocs.io/en/latest/reference.html#ytmusicapi.YTMusic.get_artist
    artist::Artist get_artist(const std::string &channel_id) const;

    /// https://ytmusicapi.readthedocs.io/en/latest/reference.html#ytmusicapi.YTMusic.get_album
    album::Album get_album(const std::string &browse_id) const;

    /// https://ytmusicapi.readthedocs.io/en/latest/reference.html#ytmusicapi.YTMusic.get_song
    song::Song get_song(const std::string &video_id) const;

    /// https://ytmusicapi.readthedocs.io/en/latest/reference.html#ytmusicapi.YTMusic.get_playlist
    playlist::Playlist get_playlist(const std::string &playlist_id, int limit = 100) const;

    /// https://ytmusicapi.readthedocs.io/en/latest/reference.html#ytmusicapi.YTMusic.get_artist_albums
    std::vector<artist::Artist::Album> get_artist_albums(const std::string &channel_id, const std::string &params) const;

    /// youtube-dl's extract_info function
    video_info::VideoInfo extract_video_info(const std::string &video_id) const;

    /// https://ytmusicapi.readthedocs.io/en/latest/reference.html#ytmusicapi.YTMusic.get_watch_playlist
    watch::Playlist get_watch_playlist(const std::optional<std::string> &videoId = std::nullopt,
                                      const std::optional<std::string> &playlistId = std::nullopt,
                                      int limit = 25,
                                      const std::optional<std::string> &params = std::nullopt) const;

    // TODO wrap more methods

private:
    std::unique_ptr<YTMusicPrivate> d;
};

#endif // YTMUSIC_H
