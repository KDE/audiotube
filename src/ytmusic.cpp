// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ytmusic.h"

#include <algorithm>

#include <pybind11/embed.h>
#include <pybind11/stl.h>

namespace py = pybind11;

using namespace py::literals;

#define UNEXPORT __attribute__ ((visibility("hidden")))

struct UNEXPORT YTMusicPrivate {
    py::scoped_interpreter guard {};
    py::object ytmusic;
    py::object ytdl = py::none();
};

template <typename T>
std::optional<T> optional_key(py::handle obj, const char *name) {
    if (!obj.cast<py::dict>().contains(name)) {
        return std::nullopt;
    }

    return obj[name].cast<T>();
}

meta::Thumbnail extract_thumbnail(py::handle thumbnail) {
    return {
        thumbnail["url"].cast<std::string>(),
        thumbnail["width"].cast<int>(),
        thumbnail["height"].cast<int>()
    };
}

meta::Artist extract_meta_artist(py::handle artist) {
    return {
        artist["name"].cast<std::string>(),
        artist["id"].cast<std::optional<std::string>>()
    };
};

album::Track extract_album_track(py::handle track) {
    return {
        track["index"].cast<std::string>(),
        track["title"].cast<std::string>(),
        track["artists"].cast<std::string>(),
        track["videoId"].cast<std::optional<std::string>>(),  // E rated songs don't have a videoId
        track["lengthMs"].cast<std::optional<std::string>>(), //
        track["likeStatus"].cast<std::optional<std::string>>()
    };
}

video_info::Format extract_format(py::handle format) {
    return {
        format["quality"].cast<int>(),
        format["url"].cast<std::string>(),
        format["vcodec"].cast<std::string>(),
        format["acodec"].cast<std::string>()
    };
}

playlist::Track extract_playlist_track(py::handle track);
watch::Playlist::Track extract_watch_track(py::handle track);

template <typename T>
inline auto extract_py_list(py::handle obj) {
    const auto list = obj.cast<py::list>();
    std::vector<T> output;

    std::transform(list.begin(), list.end(), std::back_inserter(output), [](py::handle item) {
        if constexpr(std::is_same_v<T, meta::Thumbnail>) {
            return extract_thumbnail(item);
        } else if constexpr(std::is_same_v<T, meta::Artist>) {
            return extract_meta_artist(item);
        } else if constexpr(std::is_same_v<T, album::Track>) {
            return extract_album_track(item);
        } else if constexpr(std::is_same_v<T, playlist::Track>) {
            return extract_playlist_track(item);
        } else if constexpr(std::is_same_v<T, video_info::Format>) {
            return extract_format(item);
        } else if constexpr(std::is_same_v<T, watch::Playlist::Track>) {
            return extract_watch_track(item);
        } else {
            return item.cast<T>();
        }
    });

    return output;
}

meta::Album extract_meta_album(py::handle album) {
    return meta::Album {
        album["name"].cast<std::string>(),
        album["id"].cast<std::optional<std::string>>()
    };
}

watch::Playlist::Track extract_watch_track(py::handle track) {
    return {
        track["title"].cast<std::string>(),
        track["length"].cast<std::optional<std::string>>(),
        track["videoId"].cast<std::string>(),
        track["playlistId"].cast<std::string>(),
        extract_py_list<meta::Thumbnail>(track["thumbnail"]),
        track["likeStatus"].cast<std::optional<std::string>>(),
        extract_py_list<meta::Artist>(track["artists"]),
        [&]() -> std::optional<meta::Album> {
            if (!track.cast<py::dict>().contains("album")) {
                return std::nullopt;
            }

            return extract_meta_album(track["album"]);
        }()
    };
}


playlist::Track extract_playlist_track(py::handle track) {
    return {
        track["videoId"].cast<std::optional<std::string>>(),
        track["title"].cast<std::string>(),
        extract_py_list<meta::Artist>(track["artists"]),
        [&]() -> std::optional<meta::Album> {
            if (track["album"].is_none()) {
                return std::nullopt;
            }

            return extract_meta_album(track["album"]);
        }(),
        track["duration"].cast<std::string>(),
        track["likeStatus"].cast<std::optional<std::string>>(),
        extract_py_list<meta::Thumbnail>(track["thumbnails"]),
        track["isAvailable"].cast<bool>(),
        track["isExplicit"].cast<bool>()
    };
}

artist::Artist::Song::Album extract_song_album(py::handle album) {
    return {
        album["name"].cast<std::string>(),
        album["id"].cast<std::string>()
    };
};

template <typename T>
auto extract_artist_section_results(py::handle section) {
    if (!section.cast<py::dict>().contains("results")) {
        return std::vector<T>();
    }

    const py::list py_results = section["results"];
    std::vector<T> results;
    std::transform(py_results.begin(), py_results.end(), std::back_inserter(results), [](py::handle result) {
        if constexpr(std::is_same_v<T, artist::Artist::Song>) {
            return artist::Artist::Song {
                result["videoId"].cast<std::string>(),
                result["title"].cast<std::string>(),
                extract_py_list<meta::Thumbnail>(result["thumbnails"]),
                extract_py_list<meta::Artist>(result["artists"]),
                extract_song_album(result["album"])
            };
        } else if constexpr(std::is_same_v<T, artist::Artist::Album>) {
            return artist::Artist::Album {
                result["title"].cast<std::string>(),
                extract_py_list<meta::Thumbnail>(result["thumbnails"]),
                result["year"].cast<std::string>(),
                result["browseId"].cast<std::string>(),
                std::nullopt
            };
        } else if constexpr(std::is_same_v<T, artist::Artist::Single>) {
            return artist::Artist::Single {
                result["title"].cast<std::string>(),
                extract_py_list<meta::Thumbnail>(result["thumbnails"]),
                result["year"].cast<std::string>(),
                result["browseId"].cast<std::string>()
            };
        } else if constexpr(std::is_same_v<T, artist::Artist::Video>) {
            return artist::Artist::Video {
                result["title"].cast<std::string>(),
                extract_py_list<meta::Thumbnail>(result["thumbnails"]),
                optional_key<std::string>(result, "views"),
                result["videoId"].cast<std::string>(),
                result["playlistId"].cast<std::string>()
            };
        } else {
            Py_UNREACHABLE();
        }
    });

    return results;
}

template<typename T>
std::optional<artist::Artist::Section<T>> extract_artist_section(py::handle artist, const char* name) {
    if (artist.cast<py::dict>().contains(name)) {
        const auto section = artist[name];
        return artist::Artist::Section<T> {
            section["browseId"].cast<std::optional<std::string>>(),
            extract_artist_section_results<T>(section),
            optional_key<std::string>(section, "params")
        };
    } else {
        return std::nullopt;
    }
}

std::optional<search::SearchResultItem> extract_search_result(py::handle result) {
    const auto resultType = result["resultType"].cast<std::string>();
    if (resultType == "video") {
        return search::Video {
            {
                result["videoId"].cast<std::string>(),
                result["title"].cast<std::string>(),
                extract_py_list<meta::Artist>(result["artists"]),
                result["views"].cast<std::string>()
            },
            result["duration"].cast<std::string>()
        };
    } else if (resultType == "song") {
        return search::Song {
            {
                result["videoId"].cast<std::string>(),
                result["title"].cast<std::string>(),
                extract_py_list<meta::Artist>(result["artists"]),
                result["duration"].cast<std::string>()
            },
            extract_meta_album(result["album"]),
            result["isExplicit"].cast<bool>()
        };
    } else if (resultType == "album") {
        return search::Album {
            result["browseId"].cast<std::string>(),
            result["title"].cast<std::string>(),
            result["type"].cast<std::string>(),
            extract_py_list<meta::Artist>(result["artists"]),
            result["year"].cast<std::string>(),
            result["isExplicit"].cast<bool>()
        };
    } else if (resultType == "playlist") {
        return search::Playlist {
            result["browseId"].cast<std::string>(),
            result["title"].cast<std::string>(),
            result["author"].cast<std::string>(),
            result["itemCount"].cast<std::string>()
        };
    } else if (resultType == "artist") {
        return search::Artist {
            result["browseId"].cast<std::string>(),
            result["artist"].cast<std::string>(),
            optional_key<std::string>(result, "shuffleId"),
            optional_key<std::string>(result, "radioId")
        };
    } else {
        std::cerr << "Warning: Unsupported search result type found" << std::endl;
        std::cerr << "It's called: " << resultType;
        return std::nullopt;
    }
}

YTMusic::YTMusic(
        const std::optional<std::string> &auth,
        const std::optional<std::string> &user,
        const std::optional<bool> requests_session,
        const std::optional<std::map<std::string, std::string> > &proxies,
        const std::string &language)
    : d(std::make_unique<YTMusicPrivate>())
{
    const auto module = py::module::import("ytmusicapi");
    d->ytmusic = module.attr("YTMusic")(auth, user, requests_session, proxies, language);

    setenv("LC_ALL", "en_US.utf8", true);

    const auto version = module.attr("_version").attr("__version__").cast<std::string>();
    if (version != "0.14.3") {
        std::cerr << "Running with untested version of ytmusicapi." << std::endl;
        std::cerr << "The currently tested and supported version is 0.14.3" << std::endl;
    }
}

YTMusic::~YTMusic() = default;

std::vector<search::SearchResultItem> YTMusic::search(
        const std::string &query,
        const std::optional<std::string> &filter,
        const int limit,
        const bool ignore_spelling) const
{
    const auto results = d->ytmusic.attr("search")(query, filter, limit, ignore_spelling).cast<py::list>();

    std::vector<search::SearchResultItem> output;
    for (const auto &result : results) {
        if (const auto opt = extract_search_result(result); opt.has_value()) {
            output.push_back(opt.value());
        }
    };

    return output;
}

artist::Artist YTMusic::get_artist(const std::string &channel_id) const
{
    const auto artist = d->ytmusic.attr("get_artist")(channel_id);
    return artist::Artist {
        artist["description"].cast<std::optional<std::string>>(),
        artist["views"].cast<std::optional<std::string>>(),
        artist["name"].cast<std::string>(),
        artist["channelId"].cast<std::string>(),
        artist["subscribers"].cast<std::string>(),
        artist["subscribed"].cast<bool>(),
        extract_py_list<meta::Thumbnail>(artist["thumbnails"]),
        extract_artist_section<artist::Artist::Song>(artist, "songs"),
        extract_artist_section<artist::Artist::Album>(artist, "albums"),
        extract_artist_section<artist::Artist::Single>(artist, "singles"),
        extract_artist_section<artist::Artist::Video>(artist, "videos"),
    };
}

album::Album YTMusic::get_album(const std::string &browseId) const
{
    const auto album = d->ytmusic.attr("get_album")(browseId);

    return {
        album["title"].cast<std::string>(),
        album["trackCount"].cast<std::string>(),
        album["durationMs"].cast<std::string>(),
        album["playlistId"].cast<std::string>(),
        [&]() -> album::Album::ReleaseDate {
            const auto date = album["releaseDate"];
            return {
                date["year"].cast<int>(),
                date["month"].cast<int>(),
                date["day"].cast<int>()
            };
        }(),
        album["description"].cast<std::string>(),
        extract_py_list<meta::Thumbnail>(album["thumbnails"]),
        extract_py_list<album::Track>(album["tracks"])
    };
}

song::Song YTMusic::get_song(const std::string &video_id) const
{
    const auto song = d->ytmusic.attr("get_song")(video_id);

    return {
        song["videoId"].cast<std::string>(),
        song["title"].cast<std::string>(),
        song["lengthSeconds"].cast<std::string>(),
        extract_py_list<std::string>(song["keywords"]),
        song["channelId"].cast<std::string>(),
        song["isOwnerViewing"].cast<bool>(),
        song["shortDescription"].cast<std::string>(),
        song["isCrawlable"].cast<bool>(),
        song::Song::Thumbnail {
            extract_py_list<meta::Thumbnail>(song["thumbnail"]["thumbnails"])
        },
        song["averageRating"].cast<float>(),
        song["allowRatings"].cast<bool>(),
        song["viewCount"].cast<std::string>(),
        song["author"].cast<std::string>(),
        song["isPrivate"].cast<bool>(),
        song["isUnpluggedCorpus"].cast<bool>(),
        song["isLiveContent"].cast<bool>(),
        song["provider"].cast<std::string>(),
        extract_py_list<std::string>(song["artists"]),
        song["copyright"].cast<std::string>(),
        extract_py_list<std::string>(song["production"]),
        song["release"].cast<std::string>(),
        song["category"].cast<std::string>()
    };
}

playlist::Playlist YTMusic::get_playlist(const std::string &playlist_id, int limit) const
{
    const auto playlist = d->ytmusic.attr("get_playlist")(playlist_id, limit);

    return {
        playlist["id"].cast<std::string>(),
        playlist["privacy"].cast<std::string>(),
        playlist["title"].cast<std::string>(),
        extract_py_list<meta::Thumbnail>(playlist["thumbnails"]),
        extract_meta_artist(playlist["author"]),
        playlist["year"].cast<std::string>(),
        playlist["duration"].cast<std::string>(),
        playlist["trackCount"].cast<int>(),
        extract_py_list<playlist::Track>(playlist["tracks"]),
    };
}

std::vector<artist::Artist::Album> YTMusic::get_artist_albums(const std::string &channel_id, const std::string &params) const
{
    const auto py_albums = d->ytmusic.attr("get_artist_albums")(channel_id, params);
    std::vector<artist::Artist::Album> albums;

    std::transform(py_albums.begin(), py_albums.end(), std::back_inserter(albums), [](py::handle album) {
        return artist::Artist::Album {
            album["title"].cast<std::string>(),
            extract_py_list<meta::Thumbnail>(album["thumbnails"]),
            album["year"].cast<std::string>(),
            album["browseId"].cast<std::string>(),
            album["type"].cast<std::string>()
        };
    });

    return albums;
}

video_info::VideoInfo YTMusic::extract_video_info(const std::string &video_id) const
{
    using namespace pybind11::literals;

    // lazy initialization
    if (d->ytdl.is_none()) {
        const auto module = py::module::import("youtube_dl");
        py::dict options;
        d->ytdl = module.attr("YoutubeDL")(options);
    }

    const auto info = d->ytdl.attr("extract_info")(video_id, "download"_a=py::bool_(false));
    //py::print(py::module::import("json").attr("dumps")(info, "indent"_a=4));

    return {
        info["id"].cast<std::string>(),
        info["title"].cast<std::string>(),
        extract_py_list<video_info::Format>(info["formats"]),
        info["thumbnail"].cast<std::string>()
    };
}

watch::Playlist YTMusic::get_watch_playlist(const std::optional<std::string> &videoId,
                                            const std::optional<std::string> &playlistId,
                                            int limit,
                                            const std::optional<std::string> &params) const
{
    const auto playlist = d->ytmusic.attr("get_watch_playlist")(videoId, playlistId, limit, params);

    return {
        extract_py_list<watch::Playlist::Track>(playlist["tracks"]),
        playlist["lyrics"].cast<std::optional<std::string>>()
    };
}
