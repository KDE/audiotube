// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
#pragma once

#include <QAbstractListModel>
#include <QFuture>

#include <ytmusic.h>

#include "abstractytmusicmodel.h"

class UserPlaylistModel : public AbstractYTMusicModel
{
    Q_OBJECT

    // input
    Q_PROPERTY(QString initialVideoId READ initialVideoId WRITE setInitialVideoId NOTIFY initialVideoIdChanged)
    Q_PROPERTY(QString playlistId READ playlistId WRITE setPlaylistId NOTIFY playlistIdChanged)
    Q_PROPERTY(bool shuffle READ shuffle WRITE setShuffle NOTIFY shuffleChanged)

    // output
    Q_PROPERTY(QString currentVideoId READ currentVideoId NOTIFY currentVideoIdChanged)
    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentVideoIdChanged)
    Q_PROPERTY(bool canSkip READ canSkip NOTIFY canSkipChanged)
    Q_PROPERTY(bool canSkipBack READ canSkipBack NOTIFY canSkipBackChanged)
    Q_PROPERTY(QString lyrics READ lyrics NOTIFY lyricsChanged)

public:
    enum Role {
        Title = Qt::UserRole + 1,
        VideoId,
        Artists,
        Album,
        IsCurrent,
    };
    Q_ENUM(Role);

    explicit UserPlaylistModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString initialVideoId() const;
    void setInitialVideoId(const QString &videoId);
    Q_SIGNAL void initialVideoIdChanged();

    QString nextVideoId() const;
    QString previousVideoId() const;

    QString currentVideoId() const;
    void setCurrentVideoId(const QString &videoId);
    Q_SIGNAL void currentVideoIdChanged();

    int currentIndex() const;

    bool canSkip() const;
    bool canSkipBack() const;

    Q_SIGNAL void canSkipChanged();
    Q_SIGNAL void canSkipBackChanged();


    QString playlistId() const;
    void setPlaylistId(const QString &playlistId);
    Q_SIGNAL void playlistIdChanged();

    void setShuffle(bool shuffle);
    bool shuffle() const;
    Q_SIGNAL void shuffleChanged();

    QString lyrics() const;
    Q_SIGNAL void lyricsChanged();

    Q_INVOKABLE void next();
    Q_INVOKABLE void previous();

    Q_INVOKABLE void skipTo(const QString &videoId);
    Q_INVOKABLE void playNext(const QString &videoId, const QString &title, const std::vector<meta::Artist> &artists);
    Q_INVOKABLE void append(const QString &videoId, const QString &title, const std::vector<meta::Artist> &artists);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void clearExceptCurrent();
    Q_INVOKABLE void remove(const QString &videoId);
    Q_INVOKABLE void shufflePlaylist();

private:
    void emitCurrentVideoChanged(const QString &oldVideoId);

    void fetchLyrics(const QString &videoId);

    QString m_initialVideoId;
    QString m_playlistId;
    QString m_currentVideoId;
    bool m_shuffle = false;

    watch::Playlist m_playlist;
    ::Lyrics m_lyrics;
};
