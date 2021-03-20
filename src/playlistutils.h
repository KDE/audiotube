// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <vector>

class QString;
namespace meta {
struct Artist;
}

class PlaylistUtils
{
public:
    PlaylistUtils() = delete;

    static QString artistsToString(const std::vector<meta::Artist> &artists);
};
