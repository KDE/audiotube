// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "playlistutils.h"

#include <QString>

#include <numeric>

#include <ytmusic.h>

namespace PlaylistUtils {

QString artistsToString(const std::vector<meta::Artist> &artists)
{
    if (!artists.empty()) {
        QString string = QString::fromStdString(artists.front().name);
        std::for_each(artists.begin() + 1, artists.end(),
                               [&string](const meta::Artist &artist) {
            string.append(QStringLiteral(", %1").arg(QString::fromStdString(artist.name)));
        });
        return string;
    }

    return {};
}

}
