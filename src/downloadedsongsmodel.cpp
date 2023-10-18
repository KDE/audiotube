// SPDX-FileCopyrightText: 2023 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "downloadedsongsmodel.h"

#include "downloadmanager.h"
#include "library.h"

namespace ranges = std::ranges;

DownloadedSongsModel::DownloadedSongsModel(QObject *parent)
    : QAbstractListModel{parent}
{
    auto update = [this]() {
        auto future = Library::instance().downloadedSongs();
        QCoro::connect(std::move(future), this, [this](auto &&songs) {
            beginResetModel();
            m_songs = std::move(songs);
            endResetModel();
        });
    };

    update();
}
