// SPDX-FileCopyrightText: 2023 Théophile Gilgien <theophile@gilgien.net>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include "ytmusic.h"
#include <QObject>
#include <ThreadedDatabase>
#include <qqmlintegration.h>

class PlaylistImporter : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    PlaylistImporter(QObject *parent = nullptr);

    Q_INVOKABLE void importPlaylist(const QString &url);
    Q_SIGNAL void importFinished();
    Q_INVOKABLE void addPlaylistEntry(qint64 playlistId, const QString &videoId, const QString &title, const QString &artist, const QString &album);
    Q_INVOKABLE void addPlaylistEntry(qint64 playlistId, const playlist::Track &track);

    Q_INVOKABLE void renamePlaylist(qint64 playlistId, const QString &name, const QString &description);

    Q_SIGNAL void playlistEntriesChanged(quint64 playlistId);
	Q_SIGNAL void refreshModel();

private:
    QStringView cropURL(QStringView srcURL);
};
