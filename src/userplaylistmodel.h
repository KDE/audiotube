// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
#pragma once

#include <QAbstractListModel>

#include <ytmusic.h>

class UserPlaylistModel : public QAbstractListModel
{
    Q_OBJECT

    // input
    Q_PROPERTY(QString initialVideoId READ initialVideoId WRITE setInitialVideoId NOTIFY initialVideoIdChanged)
    Q_PROPERTY(QString playlistId READ playlistId WRITE setPlaylistId NOTIFY playlistIdChanged)
    Q_PROPERTY(bool shuffle READ shuffle WRITE setShuffle NOTIFY shuffleChanged)

    // output
    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)
    Q_PROPERTY(QString currentVideoId READ currentVideoId NOTIFY currentVideoIdChanged)
    Q_PROPERTY(bool canSkip READ canSkip NOTIFY canSkipChanged)

public:
    enum Role {
        Title = Qt::UserRole + 1,
        VideoId,
        Artists,
        IsCurrent
    };

    explicit UserPlaylistModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString initialVideoId() const;
    void setInitialVideoId(const QString &videoId);
    Q_SIGNAL void initialVideoIdChanged();

    bool loading() const;
    void setLoading(bool loading);
    Q_SIGNAL void loadingChanged();

    QString nextVideoId() const;

    QString currentVideoId() const;
    void setCurrentVideoId(const QString &videoId);
    Q_SIGNAL void currentVideoIdChanged();

    bool canSkip() const;
    Q_SIGNAL void canSkipChanged();

    QString playlistId() const;
    void setPlaylistId(const QString &playlistId);
    Q_SIGNAL void playlistIdChanged();

    void setShuffle(bool shuffle);
    bool shuffle() const;
    Q_SIGNAL void shuffleChanged();

    Q_INVOKABLE void next();
    Q_INVOKABLE void skipTo(const QString &videoId);
    Q_INVOKABLE void playNext(const QString &videoId, const QString &title, const std::vector<meta::Artist> &artists);
    Q_INVOKABLE void append(const QString &videoId, const QString &title, const std::vector<meta::Artist> &artists);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void remove(const QString &videoId);
    Q_INVOKABLE void shufflePlaylist();

private:
    void emitCurrentVideoChanged(const QString &oldVideoId);

    QString m_initialVideoId;
    QString m_playlistId;
    QString m_currentVideoId;
    bool m_loading = false;
    bool m_shuffle = false;

    watch::Playlist m_playlist;
};
