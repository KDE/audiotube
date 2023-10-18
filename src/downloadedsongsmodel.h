// SPDX-FileCopyrightText: 2023 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QAbstractListModel>

#include "library.h"

class DownloadedSongsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit DownloadedSongsModel(QObject *parent = nullptr);

    enum Role {
        Title,
        Artist,
        VideoId
    };

    QHash<int, QByteArray> roleNames() const override {
        return {
            { Title, "title" },
            { Artist, "artist" },
            { VideoId, "videoId" }
        };
    }

    int rowCount(const QModelIndex &parent) const override {
        return parent.isValid() ? 0 : m_songs.size();
    }

    QVariant data(const QModelIndex &index, int role) const override {
        auto &song = m_songs[index.row()];

        switch (role) {
        case Role::Title:
            return song.title;
        case Role::Artist:
            return song.artist;
        case Role::VideoId:
            return song.videoId;
        }

        Q_UNREACHABLE();

        return {};
    }

private:
    std::vector<Song> m_songs;
};

