// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

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
