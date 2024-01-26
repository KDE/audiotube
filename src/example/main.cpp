// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include <ytmusic.h>

#include <iostream>

int main() {
    const auto ytm = YTMusic();
    const auto results = ytm.search("Systemabsturz");

    std::cout << "Found " << results.size() << " results." << std::endl;

    for (const auto &result : results) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, search::Album>) {
                std::cout << arg.title << std::endl;
                if (arg.browse_id) {
                    const album::Album album = ytm.get_album(*arg.browse_id);
                    for (const auto &track : album.tracks) {
                        if (track.video_id.has_value()) {
                            ytm.extract_video_info(*track.video_id);
                        }
                    }
                }
            } else if constexpr (std::is_same_v<T, search::Artist>) {
                std::cout << arg.artist << std::endl;
                const artist::Artist artist = ytm.get_artist(arg.browse_id);
                if (artist.albums->params) {
                    const auto albums = ytm.get_artist_albums(artist.channel_id, *artist.albums->params);
                } else {
                    std::cerr << " ! Album params not available" << std::endl;
                }
            } else if constexpr (std::is_same_v<T, search::Playlist>) {
                std::cout << arg.title << std::endl;
                ytm.get_playlist(arg.browse_id);
            } else if constexpr (std::is_same_v<T, search::Song>) {
                if (!ytm.get_song(arg.video_id)) {
                    std::cout << "Song info was empty" << std::endl;
                }
                ytm.extract_video_info(arg.video_id);
                ytm.get_watch_playlist(arg.video_id);
                std::cout << arg.title << std::endl;
            } else if constexpr (std::is_same_v<T, search::Video>) {
                std::cout << arg.title << std::endl;
            } else if constexpr (std::is_same_v<T, search::TopResult>) {
                if (arg.title) {
                    std::cout << *arg.title << std::endl;
                }
                if (arg.video_id) {
                    ytm.extract_video_info(*arg.video_id);
                    auto playlist = ytm.get_watch_playlist(*arg.video_id);
                    if (playlist.lyrics) {
                        ytm.get_lyrics(*playlist.lyrics);
                    }
                } else if (!arg.artists.empty()) {
                    for (const meta::Artist &artist : arg.artists) {
                        if (artist.id) {
                            ytm.get_artist(*artist.id);
                        }
                    }
                }
            } else {
                std::cout << "non exaustive visitor" << std::endl;
            }
        }, result);
    }

    std::flush(std::cout);
}
